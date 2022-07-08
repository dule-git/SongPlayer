# SongPlayer

The project is an embedded system with an STM32F103R6 chip that is first configured to program an external memory with a song.
Firstly, a song is compressed with python code using Huffman Coding algorithm so that the song can be put into a 2MB external memory.
Huffman tree is then written into the configuration of the chip.
With the usage of UART protocol, the compressed song is transferred from the computer to the chip, and from the chip to the external memory.
After that initial step, the chip reads data from the memory, decompresses it and outputs the data to a speaker so the song can be played.
