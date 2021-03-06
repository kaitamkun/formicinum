# Formicine
A simple C++ library that wraps ostreams for use with ANSI escapes.

### Example
##### Code
```cpp
#include <iostream>
#include "ansi.h"

int main(int, char **) {
    ansi::ansistream as(std::cout, std::cerr);

    as << "Normal text\n";
    as << ansi::bold << "Bold text" << ansi::reset << "\n";
    as << fg(ansi::green) << "Green text" << ansi::reset << "\n";
    as << ansi::underline << "Underlined text,"
       << ansi::italic << " italic+underlined text"
       >> ansi::underline << " and just italic text."
       << ansi::reset << "\n";
}
```

##### Output
> Normal text  
**Bold text**  
<span style="color:green">Green text</span>  
<span style="text-decoration:underline">Underlined text, <span style="font-style:italic">italic+underlined text</span></span> <span style="font-style:italic">and just italic text.</span>
