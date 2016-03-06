#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <conio.h>

#include <atomic>
#include <future>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

static const int FREQ = 44100;
static const int BLOCK_SIZE = FREQ / 750;
static const int MAX_BLOCKS = 200;
using Sample = int16_t;
static const auto FRAME_TIME = std::chrono::seconds(1) / 750;

#define C0 16.35
#define Db0 17.32
#define D0 18.35
#define Eb0 19.45
#define E0 20.60
#define F0 21.83
#define Gb0 23.12
#define G0 24.50
#define Ab0 25.96
#define A0 27.50
#define Bb0 29.14
#define B0 30.87
#define C1 32.70
#define Db1 34.65
#define D1 36.71
#define Eb1 38.89
#define E1 41.20
#define F1 43.65
#define Gb1 46.25
#define G1 49.00
#define Ab1 51.91
#define A1 55.00
#define Bb1 58.27
#define B1 61.74
#define C2 65.41
#define Db2 69.30
#define D2 73.42
#define Eb2 77.78
#define E2 82.41
#define F2 87.31
#define Gb2 92.50
#define G2 98.00
#define Ab2 103.83
#define A2 110.00
#define Bb2 116.54
#define B2 123.47
#define C3 130.81
#define Db3 138.59
#define D3 146.83
#define Eb3 155.56
#define E3 164.81
#define F3 174.61
#define Gb3 185.00
#define G3 196.00
#define Ab3 207.65
#define A3 220.00
#define Bb3 233.08
#define B3 246.94
#define C4 261.63
#define Db4 277.18
#define D4 293.66
#define Eb4 311.13
#define E4 329.63
#define F4 349.23
#define Gb4 369.99
#define G4 392.00
#define Ab4 415.30
#define A4 440.00
#define Bb4 466.16
#define B4 493.88
#define C5 523.25
#define Db5 554.37
#define D5 587.33
#define Eb5 622.25
#define E5 659.25
#define F5 698.46
#define Gb5 739.99
#define G5 783.99
#define Ab5 830.61
#define A5 880.00
#define Bb5 932.33
#define B5 987.77
#define C6 1046.50
#define Db6 1108.73
#define D6 1174.66
#define Eb6 1244.51
#define E6 1318.51
#define F6 1396.91
#define Gb6 1479.98
#define G6 1567.98
#define Ab6 1661.22
#define A6 1760.00
#define Bb6 1864.66
#define B6 1975.53
#define C7 2093.00
#define Db7 2217.46
#define D7 2349.32
#define Eb7 2489.02
#define E7 2637.02
#define F7 2793.83
#define Gb7 2959.96
#define G7 3135.96
#define Ab7 3322.44
#define A7 3520.00
#define Bb7 3729.31
#define B7 3951.07
#define C8 4186.01
#define Db8 4434.92
#define D8 4698.63
#define Eb8 4978.03
#define E8 5274.04
#define F8 5587.65
#define Gb8 5919.91
#define G8 6271.93
#define Ab8 6644.88
#define A8 7040.00
#define Bb8 7458.62
#define B8 7902.13

std::vector<double> noteTable = {
    C0, Db0, D0, Eb0, E0, F0, Gb0, G0, Ab0, A0, Bb0, B0,
    C0, Db0, D0, Eb0, E0, F0, Gb0, G0, Ab0, A0, Bb0, B0,
    C1, Db1, D1, Eb1, E1, F1, Gb1, G1, Ab1, A1, Bb1, B1,
    C2, Db2, D2, Eb2, E2, F2, Gb2, G2, Ab2, A2, Bb2, B2,
    C3, Db3, D3, Eb3, E3, F3, Gb3, G3, Ab3, A3, Bb3, B3,
    C4, Db4, D4, Eb4, E4, F4, Gb4, G4, Ab4, A4, Bb4, B4,
    C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5, A5, Bb5, B5,
    C6, Db6, D6, Eb6, E6, F6, Gb6, G6, Ab6, A6, Bb6, B6,
    C7, Db7, D7, Eb7, E7, F7, Gb7, G7, Ab7, A7, Bb7, B7,
    C8, Db8, D8, Eb8, E8, F8, Gb8, G8, Ab8, A8, Bb8, B8,
    C8, Db8, D8, Eb8, E8, F8, Gb8, G8
};

#define DD12 0
#define DD25 1
#define DD50 2
#define DD75 3

class Channel final
{
public:
    enum class Type
    {
        Pulse,
        Triangle,
        Noise,
        Sine,
    };

    Channel(Type type)
        : m_type(type)
    {
        for (auto& block : m_blocks)
        {
            ZeroMemory(block.data, sizeof(block.data));
            block.header = {reinterpret_cast<LPSTR>(block.data), sizeof(block.data), 0, 0, 0, 0, 0, 0};
            m_unusedBlocks.push_back(&block);
        }
    }

    ~Channel()
    {
        stop();
    }

    void start()
    {
        if (m_isRunning) return;

        WAVEFORMATEX wfx;
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = 1;
        wfx.nSamplesPerSec = FREQ;
        wfx.nAvgBytesPerSec = FREQ * sizeof(Sample);
        wfx.nBlockAlign = sizeof(Sample);
        wfx.wBitsPerSample = 8 * sizeof(Sample);
        wfx.cbSize = 0;
        auto ret = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
        assert(ret == MMSYSERR_NOERROR);

        m_isRunning = true;
        m_lastFrequency = 0.0;
    }

    void stop()
    {
        if (m_isRunning)
        {
            m_isRunning = false;
            waveOutClose(m_hWaveOut);
            m_unusedBlocks.clear();
            m_usedBlocks.clear();
        }
    }

    bool ready()
    {
        for (auto it = m_usedBlocks.begin(); it != m_usedBlocks.end();)
        {
            auto pUsedBlock = *it;
            if (waveOutUnprepareHeader(m_hWaveOut, &pUsedBlock->header, sizeof(WAVEHDR)) != WAVERR_STILLPLAYING)
            {
                it = m_usedBlocks.erase(it);
                m_unusedBlocks.push_back(pUsedBlock);
                continue;
            }
            ++it;
        }

        return !m_unusedBlocks.empty();
    }

    void play(double frequency, double volume, double duty)
    {
        if (m_unusedBlocks.empty()) return;

        auto pBlock = m_unusedBlocks.front();
        m_unusedBlocks.erase(m_unusedBlocks.begin());
        m_usedBlocks.push_back(pBlock);

        switch (m_type)
        {
            case Type::Pulse:
                if (m_lastFrequency != frequency)
                {
                    m_progress = 0.0;
                    m_lastFrequency = frequency;
                }
                break;
            case Type::Triangle:
                if (volume > 0.0)
                {
                    volume = 1.0;
                }
                break;
        }

        fillData(pBlock, frequency, volume, duty);
        waveOutPrepareHeader(m_hWaveOut, &pBlock->header, sizeof(WAVEHDR));
        waveOutWrite(m_hWaveOut, &pBlock->header, sizeof(WAVEHDR));
    }

protected:
private:
    struct Block
    {
        Sample data[BLOCK_SIZE];
        WAVEHDR header;
    };

    void fillData(Block* pBlock, double frequency, double volume, double duty)
    {
        static double lastNoise = 0.0;
        static double noiseProgress = 0.0;

        double timePerBlock = static_cast<double>(BLOCK_SIZE) / static_cast<double>(FREQ);
        double frequencyPerBlock = frequency * timePerBlock;
        double frequencyPerSample = frequencyPerBlock / static_cast<double>(BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE; ++i)
        {
            double percent = static_cast<double>(i) / static_cast<double>(BLOCK_SIZE);
            double amplitude = frequencyPerBlock * percent + m_progress;
            while (amplitude > 1.f) amplitude -= 1.f;
            while (amplitude < 0.f) amplitude += 1.f;

            switch (m_type)
            {
                case Type::Pulse:
                    amplitude = amplitude >= duty ? -1.0 : 1.0;
                    break;
                case Type::Triangle:
                    amplitude = amplitude * 4.0;
                    if (amplitude > 3.0)
                    {
                        amplitude = amplitude - 4.0;
                    }
                    else if (amplitude > 1.0)
                    {
                        amplitude = 2.0 - amplitude;
                    }
                    break;
                case Type::Sine:
                    amplitude = std::sin(amplitude * 3.1415926535897932384626433832795 * 2.0);
                    break;
                case Type::Noise:
                    noiseProgress += frequencyPerSample * 30;
                    if (noiseProgress >= 1.0)
                    {
                        noiseProgress = noiseProgress - static_cast<double>(static_cast<int>(noiseProgress));
                        lastNoise = static_cast<double>(rand() % static_cast<int>(std::numeric_limits<Sample>::max()) * 2 - static_cast<int>(std::numeric_limits<Sample>::max() * 2));
                        lastNoise /= static_cast<double>(std::numeric_limits<Sample>::max());
                    }
                    amplitude = lastNoise;
                    break;
            }

            //percent *= frequencyPerBlock * 1.0;
            //if (percent > 1.0) percent = 1.0;
            //amplitude *= m_previousVolume + (volume - m_previousVolume) * percent; // Slightly smoothing things out
            amplitude *= volume;
            auto amplitudei = static_cast<Sample>(amplitude * static_cast<double>(std::numeric_limits<Sample>::max()));
            pBlock->data[i] = amplitudei;
        }

        m_previousVolume = volume;

        m_progress += frequencyPerBlock;
        if (m_progress >= 1)
        {
            m_progress = m_progress - static_cast<double>(static_cast<int>(m_progress));
        }
    }

    Block m_blocks[MAX_BLOCKS];
    std::vector<Block*> m_unusedBlocks;
    std::vector<Block*> m_usedBlocks;

    HWAVEOUT m_hWaveOut = 0;
    Type m_type;
    double m_progress = 0.0;
    double m_lastFrequency = 0.0;
    double m_previousVolume = 0.0;
    bool m_isRunning = false;
};

struct Frame
{
    Frame(int in_vol = 0, int in_duty = DD50)
        : vol(in_vol), duty(in_duty)
    {
    }
    int vol;
    int duty;
};

struct Cell
{
    Cell(double in_freq = C4, int in_vol = 0, int in_duty = DD50)
        : freq(in_freq), vol(in_vol), duty(in_duty)
    {
    }
    Cell(double in_freq, const Frame& frame)
        : freq(in_freq), vol(frame.vol), duty(frame.duty)
    {
    }
    double freq = C4;
    int vol = 0;
    int duty = DD50;
};

struct Row
{
    Cell pulse1;
    Cell pulse2;
    Cell triangle;
    Cell noise;

    const Cell& operator[](int index) const
    {
        switch (index)
        {
            case 0:
                return pulse1;
            case 1:
                return pulse2;
            case 2:
                return triangle;
            case 3:
                return noise;
        }
        return pulse1;
    }

    Cell& operator[](int index)
    {
        switch (index)
        {
            case 0:
                return pulse1;
            case 1:
                return pulse2;
            case 2:
                return triangle;
            case 3:
                return noise;
        }
        return pulse1;
    }
};

using Track = std::vector<Row>;

void playTrack(const Track& track)
{
    Channel channels[4] = {{Channel::Type::Pulse}, {Channel::Type::Pulse}, {Channel::Type::Triangle}, {Channel::Type::Noise}};

    for (auto& channel : channels) channel.start();

    int cursor = 0;
    auto rowCount = static_cast<decltype(cursor)>(track.size());

    while (!_kbhit())
    {
        bool isReady = true;
        for (auto& channel : channels)
        {
            isReady &= channel.ready();
        }
        if (isReady)
        {
            auto& row = track[cursor % rowCount];
            for (int i = 0; i < sizeof(channels) / sizeof(Channel); ++i)
            {
                auto& channel = channels[i];
                auto& rowIns = row[i];

                double duty;
                double freq = rowIns.freq;
                double vol = static_cast<double>(rowIns.vol) / 15.0;
                switch (rowIns.duty)
                {
                    case DD12:
                        duty = 0.125;
                        break;
                    case DD25:
                        duty = 0.25;
                        break;
                    case DD50:
                        duty = 0.5;
                        break;
                    case DD75:
                        duty = 0.75;
                        break;
                }

                channel.play(freq, vol, duty);
            }
            cursor = (cursor + 1) % rowCount;
        }
    }

    for (auto& channel : channels) channel.stop();
}

struct Instrument
{
    int releaseTime;
    std::vector<Frame> frames;
};

using Instruments = std::vector<Instrument>;

struct TrackFrame
{
    bool active;
    double freq;
    int instrument;
    int vol;
    int sustain;
};

#define LARG(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 64}
#define DOUB(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 32}
#define FULL(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 16}
#define HALF(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 8}
#define SHOR(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 4}
#define NOTE(__freq__, __inst__, __vol__) {true, __freq__, __inst__, __vol__, 0}
#define ____ {false, C4, 0, 0}
#define STOP {true, C4, 0, 0}

using TrackFrames = std::vector<TrackFrame>;

Track buildTrack(const Instruments& instruments, int frameSpacing, const TrackFrames& trackFrames)
{
    Track track;

    int currentCol = 0;
    int currentRow = 0;
    for (auto& trackFrame : trackFrames)
    {
        if (trackFrame.active/* && currentCol == 3*/)
        {
            auto& instrument = instruments[trackFrame.instrument];
            int sustain = trackFrame.sustain - instrument.releaseTime;
            if (sustain < 0) sustain = 0;
            for (int i = 0; i < sustain; ++i)
            {
                if (static_cast<int>(track.size()) <= currentRow + i)
                {
                    track.push_back(Row());
                }

                auto& row = track[currentRow + i];
                auto& cell = row[currentCol];

                cell.freq = trackFrame.freq;
                cell.vol = instrument.frames[0].vol * trackFrame.vol / 15;
                cell.duty = instrument.frames[0].duty;
            }
            for (int i = 0; i < static_cast<int>(instrument.frames.size()); ++i)
            {
                if (static_cast<int>(track.size()) <= currentRow + i + sustain)
                {
                    track.push_back(Row());
                }

                auto& row = track[currentRow + i + sustain];
                auto& cell = row[currentCol];

                cell.freq = trackFrame.freq;
                cell.vol = instrument.frames[i].vol * trackFrame.vol / 15;
                cell.duty = instrument.frames[i].duty;
            }
        }

        ++currentCol;
        if (currentCol == 4)
        {
            currentRow += frameSpacing;
            while (static_cast<int>(track.size()) <= currentRow)
            {
                track.push_back(Row());
            }
            currentCol = 0;
        }
    }

    // Crop the end
    track.resize(currentRow);

    return track;
}

TrackFrames buildTrackFramesFromMidi(const std::string& filename);

int main()
{
    //Instruments instruments = {
    //    {8, {{8}, {7}, {6}, {5}, {4}, {3}, {2}, {1}}}, // 0
    //    {-100, {{8}}}, // 1
    //    {0, {{8}, {6}, {4}, {2}}}, // 2
    //    {0, {{8}, {4}}}, // 3
    //};
    /*
    auto track = buildTrack(instruments, 8, {
        NOTE(C4, 0, 8), NOTE(C3, 0, 8), NOTE(G2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(C4, 0, 8), ____, NOTE(C3, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(G4, 0, 8), NOTE(C3, 0, 8), NOTE(E3, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(G4, 0, 8), ____, NOTE(C3, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(A4, 0, 8), NOTE(C3, 0, 8), NOTE(A2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(A4, 0, 8), ____, NOTE(C3, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(G4, 0, 8), NOTE(G3, 0, 8), NOTE(G2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        ____, ____, NOTE(B2, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, NOTE(C6, 3, 15),
        NOTE(F4, 0, 8), NOTE(A3, 0, 8), NOTE(A2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(F4, 0, 8), ____, NOTE(C3, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(E4, 0, 8), NOTE(A3, 0, 8), NOTE(E3, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(E4, 0, 8), ____, FULL(C3, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(D4, 0, 8), NOTE(G3, 0, 8), NOTE(G2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        NOTE(D4, 0, 8), ____, FULL(B2, 1, 15), NOTE(C6, 3, 15),
        ____, ____, ____, ____,
        NOTE(C4, 0, 8), NOTE(C3, 0, 8), NOTE(G2, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, ____,
        ____, ____, NOTE(C3, 1, 15), NOTE(C4, 2, 15),
        ____, ____, ____, NOTE(C6, 3, 15),
    });*/

    Instruments instruments = {
        //{-1000, {{8}}}, // 2
        //{-1000, {{8}}}, // 2
        //{-1000, {{8}}}, // 2
        //{-1000, {{8}}}, // 2
        {48,
        {{8}, {8}, {8}, {8}, {8}, {8},
        {7}, {7}, {7}, {7}, {7}, {7},
        {6}, {6}, {6}, {6}, {6}, {6},
        {5}, {5}, {5}, {5}, {5}, {5},
        {4}, {4}, {4}, {4}, {4}, {4},
        {3}, {3}, {3}, {3}, {3}, {3},
        {2}, {2}, {2}, {2}, {2}, {2},
        {1}, {1}, {1}, {1}, {1}, {1}}}, // 0

        {48,
        {{8}, {8}, {8}, {8}, {8}, {8},
        {7}, {7}, {7}, {7}, {7}, {7},
        {6}, {6}, {6}, {6}, {6}, {6},
        {5}, {5}, {5}, {5}, {5}, {5},
        {4}, {4}, {4}, {4}, {4}, {4},
        {3}, {3}, {3}, {3}, {3}, {3},
        {2}, {2}, {2}, {2}, {2}, {2},
        {1}, {1}, {1}, {1}, {1}, {1}}}, // 0

        //{8, {{8}, {7}, {6}, {5}, {4}, {3}, {2}, {1}}}, // 1
        {-10000, {{8}}}, // 2

        {12,
        {{8},
        {7}, {7}, {7}, 
        {6}, {6}, {6}, 
        {5}, {5}, {5}, 
        {4}, {4}, {4}, 
        {3}, {3}, {3}, 
        {2}, {2}, {2}, 
        {1}, {1}, {1}, }}, // 0
        //{-10000, {{8}}}, // 2
        //{0, {{8}, {6}, {4}, {2}}}, // 3
        //{0, {{8}, {4}}}, // 3
    };

    auto trackFrames = buildTrackFramesFromMidi("faxanadu.mid");
    auto track = buildTrack(instruments, 1, trackFrames);

    playTrack(track);

    return 0;
}

void loadStr(char* pOut, int size, FILE* pFic)
{
    fread(pOut, size, 1, pFic);
}

uint32_t load8(FILE* pFic)
{
    uint8_t a;

    fread(&a, sizeof(a), 1, pFic);

    return a;
}

uint16_t load16(FILE* pFic)
{
    uint8_t a;
    uint8_t b;

    fread(&a, sizeof(a), 1, pFic);
    fread(&b, sizeof(b), 1, pFic);

    return (static_cast<uint16_t>(a) << 8) | static_cast<uint16_t>(b);
}

uint32_t load32(FILE* pFic)
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;

    fread(&a, sizeof(a), 1, pFic);
    fread(&b, sizeof(b), 1, pFic);
    fread(&c, sizeof(c), 1, pFic);
    fread(&d, sizeof(d), 1, pFic);

    return 
        (static_cast<uint32_t>(a) << 24) | 
        (static_cast<uint32_t>(b) << 16) |
        (static_cast<uint32_t>(c) << 8) |
        (static_cast<uint32_t>(d));
}

uint32_t loadLength(FILE* pFic)
{
    uint32_t value = 0;
    bool readMore = true;
    while (readMore)
    {
        auto b = load8(pFic);
        if (b & 0x80)
        {
            readMore = true;
        }
        else
        {
            readMore = false;
        }
        value <<= 7;
        value |= b & 0x7F;
    }
    return value;
}

TrackFrames buildTrackFramesFromMidi(const std::string& filename)
{
    TrackFrames trackFrames;

    FILE* pFic;
    fopen_s(&pFic, filename.c_str(), "rb");

    // Load header
    struct HeaderChunk
    {
        char HThd[4];
        uint32_t size;
        uint16_t format;
        uint16_t nbTrack;
        uint16_t division;
    } headerChunk;
    loadStr(headerChunk.HThd, 4, pFic);
    headerChunk.size = load32(pFic);
    headerChunk.format = load16(pFic);
    headerChunk.nbTrack = load16(pFic);
    headerChunk.division = load16(pFic);

    uint32_t maxTime = 0;

    std::set<int> channelSet;
    //int midiToChannels[] = { // mario
    //    5, 0, 1, 2,
    //    5, 5, 5, 5,
    //    5, 5, 5, 5,
    //    5, 5, 5, 5,
    //};
    //int midiToChannels[] = { // sm2
    //    0, 2, 1, 5,
    //    5, 5, 5, 5,
    //    5, 3, 5, 5,
    //    5, 5, 5, 5,
    //};
    int midiToChannels[] = {
        0, 1, 2, 5,
        5, 5, 5, 5,
        5, 5, 5, 5,
        5, 5, 5, 5,
    };
    //int midiToChannels[] = { // zelda
    //    0, 1, 2, 5,
    //    5, 5, 5, 5,
    //    5, 3, 5, 5,
    //    5, 5, 5, 5,
    //};

    // Load tracks
    for (auto i = 0u; i < headerChunk.nbTrack; ++i)
    {
        char MTrk[4];
        loadStr(MTrk, 4, pFic);
        auto len = load32(pFic);
        auto start = ftell(pFic);
        auto cur = start;
        uint32_t curTime = 0;

        while (cur - start < static_cast<decltype(cur)>(len))
        {
            auto v_time = loadLength(pFic);
            auto event = load8(pFic);

            curTime += v_time;// / (headerChunk.division / 24);
            maxTime = std::max(maxTime, curTime);
            trackFrames.resize(maxTime * 4 + 16);

            if (event == 0xFF)
            {
                // Meta event
                event = load8(pFic);
                if (event == 0x58)
                {
                    // Time Signature
                    event = load8(pFic);
                    assert(event == 0x04);
                    load32(pFic); // Ignore who fucking cares
                }
                else if (event == 0x7F)
                {
                    // Sequencer Specific Meta-Event
                    auto dataLen = load8(pFic); // Len, ignore
                    while (dataLen)
                    {
                        load8(pFic);
                        --dataLen;
                    }
                }
                else if (event == 0x51)
                {
                    // Set Tempo
                    event = load8(pFic);
                    assert(event == 0x03);
                    load8(pFic);
                    load8(pFic);
                    load8(pFic);
                }
                else if (event == 0x2F)
                {
                    // End of Track
                    event = load8(pFic);
                    assert(event == 0x00);
                }
                else if (event == 0x03)
                {
                    // Sequence/Track Name
                    auto textLen = load8(pFic);
                    char text[257] = {0};
                    loadStr(text, textLen, pFic);
                    printf("Track found: %i, %s\n", i, text);
                }
                else if (event == 0x59)
                {
                    // Key Signature
                    event = load8(pFic);
                    assert(event == 0x02);
                    auto sf = load8(pFic);
                    auto mi = load8(pFic);
                }
                else if (event == 0x21)
                {
                    // End of Track
                    event = load8(pFic);
                    assert(event == 0x01);
                    load8(pFic);
                }
                else if (event == 0x20)
                {
                    // MIDI Channel Prefix
                    event = load8(pFic);
                    assert(event == 0x01);
                    auto channel = load8(pFic);
                }
                else
                {
                    assert(false);
                }
            }
            else if (event & 0x80)
            {
                auto channelNo = event & 0x0F;
                channelSet.insert(channelNo);
                if (channelNo < sizeof(midiToChannels) / sizeof(int)) channelNo = midiToChannels[channelNo];
                event = event & 0xF0;

                if (event == 0xB0)
                {
                    // Control Change
                    auto controllerNumber = load8(pFic) & 0x7F;
                    auto controllerValue = load8(pFic) & 0x7F;
                }
                else if (event == 0xC0)
                {
                    // Program Change
                    auto programNumber = load8(pFic) & 0x7F;
                }
                else if (event == 0x90)
                {
                    // Note On event
                    auto note = load8(pFic) & 0x7F;
                    auto vol = load8(pFic) & 0x7F;
                    if (channelNo < 3)
                    {
                        auto& trackFrame = trackFrames[curTime * 4 + channelNo];
                        trackFrame = NOTE(noteTable[note], channelNo, (vol * 15) / 127);
                    }
                    else if (channelNo == 3)
                    {
                        auto& trackFrame = trackFrames[curTime * 4 + channelNo];
                        trackFrame = NOTE(noteTable[note + 12], channelNo, (vol * 15) / 127);
                    }
                }
                else if (event == 0x80)
                {
                    // Note Off event
                    auto note = load8(pFic) & 0x7F;
                    auto vol = load8(pFic) & 0x7F;
                    if (channelNo == 2)
                    {
                        //auto& trackFrame = trackFrames[curTime * 4 + channelNo];
                        //trackFrame = NOTE(noteTable[note + 12], channelNo, 0);
                    }
                    if (channelNo == 3)
                    {
                        auto& trackFrame = trackFrames[curTime * 4 + channelNo];
                        trackFrame = NOTE(noteTable[note], channelNo, 0);
                    }
                }
                else if (event == 0xE0)
                {
                    // Pitch Wheel Change
                    auto l = load8(pFic) & 0x7F;
                    auto m = load8(pFic) & 0x7F;
                }
                else
                {
                    assert(false);
                }
            }
            else if (event <= 0x7F)
            {
                load8(pFic);
            }
            else
            {
                assert(false);
            }

            cur = ftell(pFic);
        }
    }

    fclose(pFic);

    for (auto& channelNo : channelSet)
    {
        printf("%i\n", channelNo);
    }

    return trackFrames;
}
