streaming() {
     INRES="1024x648" # input resolution
     OUTRES="800x600" # output resolution
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
     ffmpeg  -f x11grab -s "$INRES" -r "$FPS" -i ${_DISPLAY}.0 -stream_loop -1 -i ./bicycle.mp3 -map 0:v:0 -map 1:a:0 -filter_complex loop=-1 -acodec copy -f flv  \
       -vcodec libx264 -g $GOP -keyint_min $GOPMIN -b:v $CBR -minrate $CBR -maxrate $CBR -pix_fmt yuv420p\
       -s $OUTRES -preset $QUALITY -tune film -threads $THREADS -strict normal \
       -bufsize $CBR "rtmp://$SERVER.twitch.tv/app/$STREAM_KEY"
}

streaming $1
