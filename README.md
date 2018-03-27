Requirements:
docker (non-root user should be able to call )
docker-compose (version >= 1.13.0+)


Prechecks:

 user $ docker ps




```
docker-compose up
xhost +local:docker

```

open browser: http://localhost:2888 - add some key

bind to any button (for example F2) call
```docker exec -ti silvercomplete_gui_1 silvercomplete.sh```

enjoy
