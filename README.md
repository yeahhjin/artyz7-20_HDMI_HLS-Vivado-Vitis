### 🎯 Arty Z7-20 HDMI Sobel Filter Project (Vitis HLS + Vivado + Vitis)

---

### 📌 Project Overview  
- 본 프로젝트는 **Vitis HLS + Vivado + Vitis**를 이용하여 **사용자 정의 영상 처리 IP**를 FPGA에 구현하는 예제입니다.  
- 최종 목표는 **Sobel Edge Detection Filter** 구현이지만, 현재 단계에서는 HDMI 파이프라인과 AXI 인터페이스 검증을 위해 **Pass-through 모듈**로 동작을 확인하였습니다.

---

### 📝 Workflow  

#### 1. Vitis HLS  
- C/C++로 Pass-through 기반의 영상 처리 IP 설계 (**향후 Sobel Filter로 교체 예정**)  
- ✅ C Simulation (csim) 수행  
- ✅ Co-Simulation (cosim) 수행  
- ✅ RTL (Verilog) Export 완료  

#### 2. Vivado  
- ✅ **Arty Z7-20** 보드 파일 및 Digilent HDMI 예제(HW) 기반 프로젝트 구성  
- ✅ HLS로 생성된 IP를 Vivado Block Design에 추가  
- ✅ AXI 인터페이스 연결 및 Address Editor로 주소 매핑  
- ✅ HDMI In/Out 파이프라인에 Pass-through IP 삽입  

#### 3. Bitstream Generation  
- ✅ Vivado에서 Synthesis → Implementation → Bitstream 생성 완료  

#### 4. Vitis (SDK)  
- ✅ Zynq PS용 애플리케이션 프로젝트 생성  
- ✅ **VDMA 초기화** 및 HDMI Frame Buffer 관리 코드 작성  
- ✅ Pass-through 영상 출력 확인  

---

### 💻 Development Environment  
- **Board**: Digilent Arty Z7-20  
- **FPGA Device**: XC7Z020CLG400-1  
- **Toolchain**: Vitis HLS, Vivado, Vitis  
- **Reference Files**: Digilent HDMI In/Out 예제 (HW & SW)

---

### 💡 Purpose  
- 향후 **Sobel Filter** IP를 적용할 HDMI 영상 처리 시스템의 기반 구축  
- HDMI, AXI 인터페이스, VDMA 연동 기능 검증  
- **C → RTL → Vivado → Vitis → HDMI 출력**까지의 FPGA 설계 플로우 학습

---

### 📊 Test Results  
- HDMI Pass-through 영상 출력 정상 동작 확인  
- Frame Buffer 및 VDMA 연속 전송 테스트 성공  
