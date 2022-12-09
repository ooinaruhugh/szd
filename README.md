# szd
Append a zip file to another file.

> I honestly don't remember what szd stands for, so I *guess* it's now "***s***tick ***z***ip to ***d***ocument". ¯\\\_(ツ)\_/¯

NOTE: This handles zip files *very rudimentarily*. This needs tests and handling 
for all the additional header types from the specification.

I've seen before how people attach zip file with additional content to files they distribute,
e.g. source code to binaries that fly around the internet.
I was like, hey, that's reasonably easy, I could try that for myself.
So here's the probably over-engineered version, in C++, because I was in the mood to do C++ again.

# Usage
It's plain, there are no flags. Just do
```
szd target source
```
# License

The MIT License (MIT) 2022 - [Kamillo Ferry](https://github.com/ooinaruhugh/). Please have a look at the [LICENSE](LICENSE) for more details.
