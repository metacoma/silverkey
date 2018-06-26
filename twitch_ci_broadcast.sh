streaming() {
     #INRES="1024x648" # input resolution
     #OUTRES="800x600" # output resolution
     INRES="1920x1080" # input resolution
     OUTRES="1920x1080" # output resolution
     FPS="15" # target FPS
     GOP="30" # i-frame interval, should be double of FPS,
     GOPMIN="15" # min i-frame interval, should be equal to fps,
     THREADS="2" # max 6
     CBR="1000k" # constant bitrate (should be between 1000k - 3000k)
     QUALITY="ultrafast"  # one of the many FFMPEG preset
     STREAM_KEY="$1" # use the terminal command Streaming streamkeyhere to stream your video to twitch or justin
     SERVER="live-fra" # twitch server in frankfurt, see http://bashtech.net/twitch/ingest.php for list
     _DISPLAY=${DISPLAY:-0}

     #ffmpeg -f x11grab -s "$INRES" -r "$FPS" -i ${_DISPLAY}.0 -i ./bicycle.mp3 "amovie=audio.wav:loop=3" -acodec copy -f flv  \
     #ffmpeg  -f x11grab -s "$INRES" -r "$FPS" -stream_loop -1 -i ${_DISPLAY}.0 -stream_loop -1 -i ./bicycle.mp3 -acodec copy -f flv  \
     #ffmpeg  -f x11grab -s "$INRES" -r "$FPS" -stream_loop -1 -i ${_DISPLAY}.0 -i ./bicycle.mp3 -filter_complex "[0:a]afifo[a0];[0:a]afifo[a1];[0:a]afifo[a2];[a0][a1][a2]concat=n=3:v=0:a=1[a]" -map "[a]" -acodec copy -f flv  \
     #fc="[1]scale=iw/2:ih/2,[0]overlay=x=0:y=0,zmq=bind_address=tcp\\\://127.0.0.1\\\:1235 "
     fc="zmq=bind_address=tcp\\\://127.0.0.1\\\:1235 "
     AUDIO="./rhapsody_in_blue.mp3"
     AUDIO="http://ice1.somafm.com/missioncontrol-128-mp3"

     #ffmpeg -f x11grab -s "$INRES" -r "$FPS" -i ${_DISPLAY}.0 -filter_complex "$fc" -lavfi "amovie=${AUDIO}:loop=4444444,asetpts=N/SR/TB" -strict experimental -f flv  \
     ffmpeg -f x11grab -s "$INRES" -r "$FPS" -i ${_DISPLAY}.0 -filter_complex "$fc" -i ${AUDIO} -acodec copy -strict experimental -f flv  \
       -vcodec libx264 -g $GOP -keyint_min $GOPMIN -b:v $CBR -minrate $CBR -maxrate $CBR -pix_fmt yuv420p\
       -s $OUTRES -preset $QUALITY -tune film -threads $THREADS -strict normal \
       -bufsize $CBR "rtmp://$SERVER.twitch.tv/app/$STREAM_KEY"
}

streaming $1
