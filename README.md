# Principle-of-Compilers
### Explainations of the repo

**Code content:**
- demo
  - demo.txt
  - demo1.txt
  - demo2.txt
  - demoQ.txt
  - demoG.txt
- wordsAnalysis.cpp
- grammarAnalysis.cpp 
- semanticAnalysis.cpp
- words_Output_File.cpp
- output_words.txt


**Note:**

1.The txt files in `demo` folder is the demo of the compiler,whitch is up to you.Here i give five demo files: `demo.txt` , `demo1.txt` , `demo2.txt` , `demoQ.txt` and `demoG.txt`.
(please don't mind the casual name~)

2.This repo includes only first **three** procedure of the compiler: **Words Analysis**, **Syntax Analysis** and **Semantic Analysis**, whitch respectively correspond to **`wordsAnalysis.cpp`**,**`grammarAnalysis.cpp`** and **`semanticAnalysis.cpp`**.Additionally,the **`words_Output_File.cpp`** is the same as **`wordsAnalysis.cpp`**,with the only difference that it prints the words analysis result to the file **`output_words.txt`.**

**3.The most important notice is the sequence of the files:**
- **Words Analysis without output file**: 
`wordsAnalysis.cpp`

- **Words Analysis with output file**: 
`words_Output_File.cpp`

- **Grammar Analysis**:
`words_Output_File.cpp` -> `grammarAnalysis.cpp`

- **Semantic Analysis**: 
`words_Output_File.cpp` -> `semanticAnalysis.cpp`


**There might some potential bugs when tested with extremely tough tasks, so just take what you need from this, like design strategy or references. **
