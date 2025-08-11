#include "sobel_filter.h"
#include <iostream>

#define IMG_W  64
#define IMG_H  48

int main() {
    AXI_STREAM in, out;

    // ������ 1�� �о�ֱ�: ���� �� TLAST=1, ó�� �ȼ� TUSER=1
    for (int r = 0; r < IMG_H; ++r) {
        for (int c = 0; c < IMG_W; ++c) {
            pixel_t p;
            ap_uint<8> R = (r*5) & 0xFF;
            ap_uint<8> G = (c*3) & 0xFF;
            ap_uint<8> B = ((r+c)*2) & 0xFF;
            p.data = ((ap_uint<24>)R << 16) | ((ap_uint<24>)G << 8) | (ap_uint<24>)B;
            p.keep = -1;
            p.strb = -1;
            p.user = (r==0 && c==0) ? 1 : 0;         // SOF
            p.last = (c==IMG_W-1) ? 1 : 0;           // EOL
            p.id   = 0;
            p.dest = 0;
            in.write(p);
        }
    }

    // DUT ����: ap_ctrl_none�� ������ ��� �� �� csim�� ��������
    // ������ �������� SOF �ȼ� 1������ �� �ְ�, DUT�� �װ� ������ return �ϵ��� �Ǿ� ����.
    {
        pixel_t p;
        p.data = 0; p.keep = -1; p.strb = -1;
        p.user = 1; p.last = 0; p.id = 0; p.dest = 0;
        in.write(p);
    }

    sobel_filter(in, out);

    int count = 0;
    while (!out.empty()) {
        pixel_t q = out.read();
        if (count < 5) std::cout << std::hex << q.data << "\n";
        count++;
    }
    std::cout << "out_count=" << count << std::endl;

    if (count != IMG_W*IMG_H) {
        std::cerr << "ERROR: count mismatch\n";
        return 1;
    }
    return 0;
}
