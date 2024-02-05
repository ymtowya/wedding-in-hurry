### Emergent Wedding

This is just a small funny project that I prepared for my friend, who happened to marry in a hurry. (Congrats anyway) Since there was not much preparation time, this project can run on MCU to play the _wedding song_ and show lovely heart beat on the LED lights (align with the rhythm), as an emergency fix to the sudden marriage.

#### Demo video

[click here](https://youtu.be/2ABvQjZ1vFM)

#### Set up

* Open the project in Keil
* Include `hex` as target output
* Compile, should see no errors nor warnings
* Bake the `music2.hex` into the MCU
* Press down `K4` key, should see the heart beat shining, and the music playing from the buzzer
* Press the key again, the music and lights shall stop

#### Tech stack

* I used interrupt scheme to play the song while monitor the keys being pressed