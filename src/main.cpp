// Required for WinMain
#include <Windows.h>
#include <mmsystem.h>

// Oak Nut include
#include "Board.h"

#include "onut/Async.h"
#include "onut/Settings.h"

#include "onut_old.h"

void init();
void update();
void render();

BoardRef pBoard;


class Channel final
{
private:
    static const int audioFrequency = 48000;
    static const int bufferCount = 6;
    static const int sampleCount = 800;

public:
    enum Type
    {
        Square,
        Triangle,
        Noise,
        PCM
    };

    Channel(Type type)
        : m_type(type)
    {
    }

    ~Channel()
    {
        stop();
    }

    void start()
    {
        WAVEFORMATEX wfx;

        /*
        * first we need to set up the WAVEFORMATEX structure.
        * the structure describes the format of the audio.
        */
        wfx.nSamplesPerSec = audioFrequency; /* sample rate */
        wfx.wBitsPerSample = 16; /* sample size */
        wfx.nChannels = 1; /* channels*/

        /*
        * WAVEFORMATEX also has other fields which need filling.
        * as long as the three fields above are filled this should
        * work for any PCM (pulse code modulation) format.
        */
        wfx.cbSize = 0; /* size of _extra_ info */
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
        wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

        /*
        * try to open the default wave device. WAVE_MAPPER is
        * a constant defined in mmsystem.h, it always points to the
        * default wave device on the system (some people have 2 or
        * more sound cards).
        */
        if (waveOutOpen(
            &m_hWaveOut,
            WAVE_MAPPER,
            &wfx,
            0,
            0,
            CALLBACK_NULL
            ) != MMSYSERR_NOERROR)
        {
            fprintf(stderr, "unable to open WAVE_MAPPER device\n");
            ExitProcess(1);
        }

        /*
        * device is now open so print the success message
        * and then close the device again.
        */
        printf("The Wave Mapper device was opened successfully!\n");

        ZeroMemory(m_blocks, sizeof(m_blocks));
        m_bPlayAudio = true;

        for (int i = 0; i < bufferCount; ++i)
        {
            playBlock(i);
        }

        m_audioThread = std::thread([this]
        {
            int playingIndex = 0;
            while (m_bPlayAudio)
            {
                waitForAudio(playingIndex);
                fillBlock(playingIndex);
                playBlock(playingIndex);
                playingIndex = (playingIndex + 1) % bufferCount;
            }
        });
    }

    void stop()
    {
        if (m_hWaveOut)
        {
            m_bPlayAudio = false;
            m_audioThread.join();
            waveOutClose(m_hWaveOut);
            m_hWaveOut = nullptr;
        }
    }

private:
    void fillBlock(int index)
    {
   /*     double frequency = 15;
        double volume = .5;
        for (int j = 0; j < bufferCount; ++j)
        {
            for (int i = 0; i < sampleCount; ++i)
            {
                auto sample = static_cast<int16_t>(std::sin(static_cast<double>(i)* 3.1415926535897932384626433832795 * 2.0 / static_cast<double>(sampleCount)* frequency) * static_cast<double>(std::numeric_limits<int16_t>::max()));
                sample = sample >= 0 ? std::numeric_limits<int16_t>::max() : std::numeric_limits<int16_t>::min();
                sample = static_cast<int16_t>(static_cast<double>(sample)* volume);
                m_blocks[j * sampleCount + i] = sample;
            }
        }*/
    }

    void playBlock(int index)
    {
        auto pHeader = m_headers + index;
        /*
        * initialise the block header with the size
        * and pointer.
        */
        ZeroMemory(pHeader, sizeof(WAVEHDR));
        pHeader->dwBufferLength = sampleCount * sizeof(int16_t);
        pHeader->lpData = reinterpret_cast<decltype(pHeader->lpData)>(m_blocks + index * sampleCount);

        /*
        * prepare the block for playback
        */
        waveOutPrepareHeader(m_hWaveOut, pHeader, sizeof(WAVEHDR));

        /*
        * write the block to the device. waveOutWrite returns immediately
        * unless a synchronous driver is used (not often).
        */
        waveOutWrite(m_hWaveOut, pHeader, sizeof(WAVEHDR));
    }

    void waitForAudio(int index)
    {
        auto pHeader = m_headers + index;

        /*
        * wait a while for the block to play then start trying
        * to unprepare the header. this will fail until the block has
        * played.
        */
        while (waveOutUnprepareHeader(m_hWaveOut, pHeader, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

    Type m_type;
    std::atomic<bool> m_bPlayAudio = false;
    HWAVEOUT m_hWaveOut; /* device handle */
    int16_t m_blocks[bufferCount * sampleCount];
    WAVEHDR m_headers[bufferCount];
    std::thread m_audioThread;
};

Channel *channels[4];

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    oSettings->setGameName("Patate Quest");
    oSettings->setResolution({1280, 800});
    oSettings->setIsResizableWindow(true);
    /*
    channels[0] = new Channel(Channel::Square);
    channels[1] = new Channel(Channel::Square);
    channels[2] = new Channel(Channel::Triangle);
    channels[3] = new Channel(Channel::Noise);

    channels[0]->start();
    */
    ORun(init, update, render);
    /*
    channels[0]->stop();
    delete channels[0];*/
}

void init()
{
    pBoard = OMake<Board>(1);
}

void update()
{
    pBoard->update();
}

void render()
{
    oRenderer->clear(OColorHex(204631));
    pBoard->render();
    //OPB->begin(OPrimitiveLineStrip);
    //for (int i = 0; i < sampleCount; ++i)
    //{
    //    OPB->draw(Vector2(static_cast<float>(i), static_cast<float>(blocks[i]) / static_cast<float>(std::numeric_limits<int16_t>::max()) * 64.f + 64.f));
    //}
    //OPB->end();
}
