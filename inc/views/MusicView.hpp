#pragma once
#include <ViewHandler.hpp>
#include <widgets/Widget.hpp>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <kissfft/kiss_fftr.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <array>

class MusicView : public View {
    public:
        MusicView();
        ~MusicView();
        void draw() override;
        int logic() override;
        void start() override;
        void close() override;
    private:
        void _fetchEvents();
        std::vector<std::unique_ptr<Widget>> _widgets;
        bool        _playing    = false;
        std::string _title      = "No track";
        std::string _artist     = "No artist";
        std::string _deviceName = "Not connected";
        bool        _connected  = false;
        Font _font;
        const std::string _fontPath = "assets/roboto.ttf";

        // Audio capture
        void captureLoop();
        pa_simple*           _pulse       = nullptr;
        std::jthread         _captureThread;
        std::atomic<bool>    _capturing   {false};

        static constexpr int kSampleRate  = 44100;
        static constexpr int kBufferSize  = 2048;  // FFT input size
        static constexpr int kBarCount    = 32;

        // FFT
        kiss_fftr_cfg        _fftCfg      = nullptr;
        std::array<float, kBufferSize>              _pcmBuffer;
        std::array<kiss_fft_cpx, kBufferSize / 2 + 1> _fftOut;
        std::mutex           _bufferMutex;

        // Bar heights — current (smoothed) and target
        std::array<float, kBarCount> _barHeights  {};
        std::array<float, kBarCount> _targetHeights {};

        static constexpr float kSmoothingFactor = 6.0f;
        static constexpr float kDecayFactor     = 3.0f;

        static constexpr int W = 1024;
        static constexpr int H = 600;
    };