# cbrainfuck
A [brainfuck](https://en.wikipedia.org/wiki/Brainfuck) interpreter, written in C for Linux.

## Brainfuck
Brainfuck is an esoteric programming language. It was created by Urban Müller in 1993. The language is Turing-complete. The language itself supports only eight commands. The structure of a Brainfuck program is a sequence of these commands, along with an array of single bytes represents the state of the program, and two streams of bytes, which are used for input and output.

### Commands
Command | Meaning
------- | ------- 
>       | Increment the data pointer
<       | Decrement the data pointer
+       | Increment the value of the data pointer
-       | Decrement the value of the data pointer
.       | Send the value of the data pointer to the output stream
,       | Receive one byte of data from the input stream and store that value in at the data pointer
[       | If the value of the data pointer is 0, jump to the matching ] instruction
]       | If the value of the data pointer is not 0, jump to the matching [ instruction

## Installation
* `git clone https://www.github.com/zabertooth/cbrainfuck`
* `cd cbrainfuck/src`
* `make install`
* `bf ../srcbf/alphabet.bf`

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
