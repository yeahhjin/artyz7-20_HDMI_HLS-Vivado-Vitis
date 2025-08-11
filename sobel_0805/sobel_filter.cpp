#include "sobel_filter.h"

#ifndef MAX_COLS
#define MAX_COLS 2048   // 보드 가로폭보다 넉넉히(1920 이상 권장)
#endif

// --- 회색화: 곱셈을 DSP로 사용 (Fmax↑) ---
static inline ap_uint<8> rgb2gray(ap_uint<8> r, ap_uint<8> g, ap_uint<8> b) {
    // BT.601 근사: Y ≈ (66*R + 129*G + 25*B + 128) >> 8
    ap_uint<16> y = 66*r + 129*g + 25*b + 128;
    return (ap_uint<8>)(y >> 8);
}

// --- Sobel 3x3: 2-스테이지 + 함수 자체 파이프라인(매 사이클 새 입력 수용) ---
static ap_uint<8> sobel_core(ap_uint<8> t2, ap_uint<8> t1, ap_uint<8> t0,
                             ap_uint<8> m2, ap_uint<8> m1, ap_uint<8> m0,
                             ap_uint<8> b2, ap_uint<8> b1, ap_uint<8> b0) {
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
#pragma HLS LATENCY  min=2 max=2
    // 1단계: 부분합(가중치 적용)
    ap_int<11> gx_l = (ap_int<11>)t0 - (ap_int<11>)t2;
    ap_int<11> gx_c = ((ap_int<11>)m0 - (ap_int<11>)m2) << 1;
    ap_int<11> gx_r = (ap_int<11>)b0 - (ap_int<11>)b2;

    ap_int<11> gy_l = (ap_int<11>)t2 - (ap_int<11>)b2;
    ap_int<11> gy_c = ((ap_int<11>)t1 - (ap_int<11>)b1) << 1;
    ap_int<11> gy_r = (ap_int<11>)t0 - (ap_int<11>)b0;

    ap_int<12> gx = gx_l + gx_c + gx_r;
    ap_int<12> gy = gy_l + gy_c + gy_r;

    // 2단계: 절대값 + 합 + 클램프 (폭 명시)
    ap_int<13> gx_w = (ap_int<13>)gx;
    ap_int<13> gy_w = (ap_int<13>)gy;
    ap_int<13> ax   = (gx_w < 0) ? (ap_int<13>)(-gx_w) : gx_w;
    ap_int<13> ay   = (gy_w < 0) ? (ap_int<13>)(-gy_w) : gy_w;
    ap_int<14> mag  = (ap_int<14>)ax + (ap_int<14>)ay;

    return (mag > 255) ? (ap_uint<8>)255 : (ap_uint<8>)mag;
}

void sobel_filter(AXI_STREAM &input, AXI_STREAM &output) {
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE ap_ctrl_none port=return

    // 곱셈은 DSP로, 3개 인스턴스 허용(공유로 II↑ 방지)
#pragma HLS bind_op    op=mul impl=dsp
#pragma HLS ALLOCATION instances=mul limit=3 operation

    // 라인버퍼 3개(명시적 분리, BRAM 매핑)
    static ap_uint<8> line0[MAX_COLS];
    static ap_uint<8> line1[MAX_COLS];
    static ap_uint<8> line2[MAX_COLS];
#pragma HLS bind_storage variable=line0 type=ram_1p impl=bram
#pragma HLS bind_storage variable=line1 type=ram_1p impl=bram
#pragma HLS bind_storage variable=line2 type=ram_1p impl=bram
#pragma HLS DEPENDENCE   variable=line0 inter false
#pragma HLS DEPENDENCE   variable=line1 inter false
#pragma HLS DEPENDENCE   variable=line2 inter false

    // 현재 쓰는 줄(bank): 0→1→2→0…
    static ap_uint<2> cur = 0;

    // 위치/수평 쉬프트 상태
    static int col = 0, row = 0;
    static ap_uint<8> t2=0, t1=0, m2=0, m1=0, b2=0, b1=0;

#ifndef __SYNTHESIS__
    static bool in_frame = false; // csim에서 프레임 하나 처리 후 종료용
#endif

    for (;;) {
#pragma HLS PIPELINE II=1

        // 합성 친화적 스트림 읽기 (empty() 금지)
        pixel_t in;
#ifndef __SYNTHESIS__
        if (!input.read_nb(in)) return;     // TB 입력 끝나면 종료
#else
        if (!input.read_nb(in)) continue;   // HW: 입력 없으면 한 사이클 대기
#endif

        // 프레임 시작(SOF)
        if (in.user) {
#ifndef __SYNTHESIS__
            if (in_frame) return;           // 다음 프레임 SOF 만나면 csim 종료
            in_frame = true;
#endif
            row = 0; col = 0;
            t2=t1=0; m2=m1=0; b2=b1=0;
        }

        // RGB → Gray (DSP 3개 사용)
        ap_uint<8> r8 = in.data.range(23,16);
        ap_uint<8> g8 = in.data.range(15, 8);
        ap_uint<8> b8 = in.data.range( 7, 0);
        ap_uint<8> gray = rgb2gray(r8, g8, b8);

        // 은행별 read2 + write1을 서로 다른 배열에 분산 → 메모리 포트 경합 제거
        ap_uint<8> top0 = 0, mid0 = 0;
        if (cur == 0) {          // read: line1/line2, write: line0
            top0 = (row >= 2) ? line1[col] : (ap_uint<8>)0;
            mid0 = (row >= 1) ? line2[col] : (ap_uint<8>)0;
        } else if (cur == 1) {   // read: line2/line0, write: line1
            top0 = (row >= 2) ? line2[col] : (ap_uint<8>)0;
            mid0 = (row >= 1) ? line0[col] : (ap_uint<8>)0;
        } else {                 // cur == 2  // read: line0/line1, write: line2
            top0 = (row >= 2) ? line0[col] : (ap_uint<8>)0;
            mid0 = (row >= 1) ? line1[col] : (ap_uint<8>)0;
        }
        ap_uint<8> bot0 = gray;

        // 수평 쉬프트 (중앙은 *_1)
        t2 = t1; t1 = top0;
        m2 = m1; m1 = mid0;
        b2 = b1; b1 = bot0;

        // Sobel (경계=0) — 2-스테이지 함수(II=1)로 계산
        ap_uint<8> yout = 0;
        if (row >= 2 && col >= 2) {
            yout = sobel_core(t2, t1, top0, m2, m1, mid0, b2, b1, bot0);
        }

        // 현재 라인 write (cur 은행)
        if      (cur == 0) line0[col] = gray;
        else if (cur == 1) line1[col] = gray;
        else               line2[col] = gray;

        // 출력: 메타데이터 그대로 전달
        pixel_t out;
        out.data = ((ap_uint<24>)yout << 16) | ((ap_uint<24>)yout << 8) | (ap_uint<24>)yout;
        out.keep = in.keep;
        out.strb = in.strb;
        out.user = in.user;   // SOF
        out.last = in.last;   // EOL
        out.id   = in.id;
        out.dest = in.dest;
        output.write(out);

        // 라인 끝에서 상태 회전
        if (in.last) {
            t2=t1=0; m2=m1=0; b2=b1=0;
            col = 0; row++;
            cur = (ap_uint<2>)((cur + 1) % 3);
        } else {
            col++;
            if (col >= MAX_COLS) col = MAX_COLS - 1; // 가드
        }
    }
}
