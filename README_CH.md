# 编译原理

### 代码库解释
[简体中文](https://github.com/Jundifang/Principle-of-Compilers/blob/main/README_CH.md)/[English](https://github.com/Jundifang/Principle-of-Compilers/blob/main/README.md)

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

1.demo文件夹中的txt文件是编译器的demo，具体内容由你决定。这里我给出五个演示文件:`demo.txt`、`demo1.txt`、`demo2.txt`、`demoQ.txt`和`demoG.txt`。

2.该仓库只包含编译器设计的前三个过程:词法分析、语法分析和语义分析，分别对应于`Words Analysis.cpp`、`grammarAnalysis.cpp`和`semanticAnalysis.cpp`。此外，`words_Output_File.cpp`与`wordsAnalysis.cpp`基本相同，唯一的区别是它会将词法分析的结果打印到文件`output_words.txt`中。

**3.最重要的点是代码运行顺序:**
- **词法分析不输出文件**: 
`wordsAnalysis.cpp`

- **词法分析输出文件**: 
`words_Output_File.cpp`

- **语法分析**:
`words_Output_File.cpp` -> `grammarAnalysis.cpp`

- **语义分析**: 
`words_Output_File.cpp` -> `semanticAnalysis.cpp`


**在测试困难的任务时可能会出现一些潜在的bug，所以各取所需，比如参考设计策略或作为参考样例等等**. 
