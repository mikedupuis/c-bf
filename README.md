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

## hello.bf
This is a "Hello World!" program written in Brainfuck.

### Program

    ++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.
    
### Breakdown:

Initialize a data index 0 to 10.

    ++++++++++
    
Create a loop that will run until data index 0 reaches 0

    [(don't worry about the loop body -)]
    
In the loop body, increment data index 1 by 7, index 2 by 10, 3 by 3, 4 by 1. Finally, return to index 0 and decrement. We initialized the loop counter to 10 above, so this loop runs 10 times.

    >+++++++>++++++++++>+++>+<<<<-

Now our data array holds the following values:

    0 70 100 30 10
    
Which correspond to the character values:

    NUL 'F' 'd' RS  \n
    
This doesn't look much like "Hello World!", but these characters are close to what we need and kept the initializer code brief. From here on we will adjust the values and print them.
First, advance to index 1, which holds the value 'F'. Increment it to 'H' and print:

    >++.

Next, advance to index 2, which holds 'd'. Increment to 'e' and print:

    >+.
    
For brevity, the rest of the program is condensed. 

Print "llo World!\n":

    +++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.

## Installation
* `git clone https://www.github.com/zabertooth/cbrainfuck`
* `cd cbrainfuck/src`
* `make install`
* `bf ../srcbf/alphabet.bf`

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
