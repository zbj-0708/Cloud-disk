#ifndef SHOWPLAYER_H
#define SHOWPLAYER_H

#include <QThread>
#include<QImage>
#include"packetqueue.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/time.h"
#include "SDL.h"
}

//VideoState 结构体里面包含了解码音频视频所有的相关信息.
class showPlayer ;
typedef struct VideoState {
    AVFormatContext *pFormatCtx;//相当于视频”文件指针”
    ///////////////音频///////////////////////////////////
    AVStream *audio_st; //音频流
    PacketQueue *audioq;//音频缓冲队列
    AVCodec *pAudioCodec;//音频解码器
    AVCodecContext *pAudioCodecCtx ;//音频解码器信息指针
    int audioStream;//音频流索引
    double audio_clock; ///<pts of last decoded frame 音频时钟
    SDL_AudioDeviceID audioID; //音频 ID
    AVFrame out_frame; //设置参数，供音频解码后的 swr_alloc_set_opts 使用。
    //////////////////////////////////////////////////////
    ///////////////视频///////////////////////////////////
    AVStream *video_st; //视频流
    PacketQueue *videoq;//视频队列
    AVCodec *pVideoCodec;//视频解码器
    AVCodecContext *pCodecCtx ;//视频解码器信息指针
    int videoStream;//视频流索引
    double video_clock; ///<pts of last decoded frame 视频时钟
    SDL_Thread *video_tid; //视频线程 id
    /////////////////////////////////////////////////////

    /// 播放控制的变量
     bool isPause;//暂停标志
     bool quit; //停止
     bool readFinished; //读线程文件读取完毕
     bool readThreadFinished; //读取线程是否结束
     bool videoThreadFinished; // 视频线程是否结束
     /////////////////////////////////////////////////////
     //// 跳转相关的变量
     int seek_req; //跳转标志 -- 读线程
     int64_t seek_pos; //跳转的位置 -- 微秒
     int seek_flag_audio;//跳转标志 -- 用于音频线程中
     int seek_flag_video;//跳转标志 -- 用于视频线程中
     double seek_time; //跳转的时间(秒) 值和 seek_pos 是一样的
     //////////////////////////////////////////////////////
     int64_t start_time; //单位 微秒
     VideoState()
     {
         audio_clock = video_clock = start_time = 0;
         videoStream = -1;
         audioStream = -1;
     }
    showPlayer* m_player;//用于调用函数
} VideoState;

enum PlayerState
{
 Playing = 0,
 Pause,
 Stop
};
class showPlayer : public QThread
{
    Q_OBJECT

public:
    explicit showPlayer(QObject *parent = 0);
    ~showPlayer();
signals:
    void SIG_GetOneImage(QImage img);
    void SIG_PlayerStateChanged(PlayerState);
    void SIG_TotalTime(qint64 uSec);
public slots:
    void run();
    void SendGetOneImage(QImage& img);

    ///播放控制
    void play();
    void pause();
    void stop( bool isWait);
    void setFileName(const QString &fileName);


    double getCurrentTime();
    int64_t getTotalTime();
    void seek(int64_t pos);
public:
    VideoState m_videoState;
    QString m_fileName;
    PlayerState m_playerState;
};

#endif // SHOWPLAYER_H
