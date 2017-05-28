# Students
Trond Lohne
Per-Morten Straume

# Repositories
Thesis repo: https://github.com/Per-Morten/imt3912_thesis  
NOX Engine fork: https://gitlab.com/per-morten.straume/nox-engine  
NOX Engine original: https://gitlab.com/suttungdigital/nox-engine  
Issue 108 branch (Benchmarks): https://github.com/Per-Morten/imt3912_dev/tree/issue_%23108  
Bitbucket old repo: https://bitbucket.org/Per-Morten/imt3912_dev  

# Languages
Throughout the project we used 4 different programming languages, these were
C++, Bash, CMake and Python. Python was only used for the git hooks,
and was not used enough to warrant a discussion.

## C++
C++ was used the most in the project, and was chosen because the existing NOX Engine was written in C++.
The main advantage of the language is the control it gives in general, both over efficiency and
memory. Additionally C++ supports writing generic code quite easily, through templates or other
mechanisms. With this support for generic code also comes a standard library with relatively efficient
implementation and good documentation.
C++ is also cross platform in its standard form, and is supported on several platforms.
Additionally the C++ language has relatively good documentation, with a helpful and professional community.
Examples include reference pages like cppreference.com, a lot of questions on stack overflow, and 
videos from conferences like cppcon.

C++ is a quite verbose language, which is positive in the way that it is explicit on what is happening,
and a bit more self documenting. However, the verbosity hinders rapid iteration, as it requires the programmer
to write more to implement functionality than a lot of other languages.
The language is also a multi-paradigm language, supporting object orientation, functional programming,
procedural programming etc.
The main advantage of this is that several approaches can be taken when solving a problem.
Certain problems might lend themselves better to different programming styles, like functional
or object oriented. Having the possibility to choose the most natural approach to solving a problem
is desirable. 
However, this also means that the code base can quickly become inconsistent in terms of programming
paradigm, not only based on the problem domains, 
but also the favored paradigm of the programmer implementing the functionality.
C++ is a compiled statically typed language, which has certain pros and cons.
The main pro is that a lot of errors can be caught at compile time, examples like misspelling
of variable names, or type violations. Additionally having the program compiled before it is run
allows for better optimizations done by the compiler, leading to better performance in many cases.
However, these compile times can harm the iteration process for programmers, especially
when the compile times grows.

C++ also has several negative aspects. The language is in general pretty hard to debug,
and does not help the programmer avoid mistakes. For example in Java indexing out of bounds
in an array is checked at runtime and throws an exception, in C++ this is labeled undefined
behavior, and basically anything can happen, the best case being an immediate crash. 
The language can also be quite error prone, especially when working with the more low level
C parts of the language.
Dealing with the more esoteric parts of the language like template meta programming
is also quite error prone, and produces obscure error messages that are hard to comprehend.
Additionally syntax is quite complex, and does not always act like the programmer
might think.

## Bash
During development bash was mainly used for developing utility functionality to ease
the development process.
The language is a scripting language, and is evaluated at runtime, rather than having
a compilation phase. Additionally there is a lot of good utility offered
in the language, which can easily be composed together to create new functionality.
This combined with a relatively simple syntax and runtime allows for rapid iteration and development of new features.

While these are all nice features, bash seems to quickly fall apart when larger scripts are introduced.
Bash has a relatively simple syntax when doing smaller operations, however, when trying to create more complicated
scripts the syntax quickly becomes complex and arcane.
For example, there are several different ways to get the value of a variable, all with subtle differences.
Do you use quotes when reading a variable (a="$b") or do you use braces (a=${b}). 
Different sources will also claim that each version of reading a variable is the correct one,
making it quite confusing to read tutorials.
Additionally there is no type safety in the language, which again makes it harder to spot errors.
Seeing that there is no compilation phase, errors happen at runtime, and are not necessarily reported,
but rather lead to weird results. Misspelling a variable can lead to unexpected behavior,
rather than being reported properly as an error.
Additionally the language does not have proper scope mechanisms, meaning that a lot of variables are reused,
or global variables are introduced to make up for the lack of return values from functions.
However, for smaller scripts and tools, these tradeoffs are usually worth it.

## CMake
CMake is a build tool, and was used extensively within the project. 
It was used to set up different compilation configurations, including benchmark configurations.
CMake was chosen because it was already used in the NOX Engine.

The main advantage of CMake is how easily it allows for cross platform builds.
Setting it up in a unix environment is pretty straight forward, and on windows
one can easily use CMake to generate visual studio projects.
The syntax of the "language" is pretty straight forward, and easy to learn.
CMake can be a valuable tool for small to medium sized projects.

There are however a few negative sides to using CMake in our experience.
Learning CMake is quite difficult, with lacking documentation and online resources.
Additionally there seems to be a lot of "magic" happening within CMake,
it felt like a lot of this magic was hidden from the programmer,
but it was still affecting the build logic.
Sometimes a particular statement worked with one directory, but not with another,
without any clear indication as to why.
We also felt that the language was questioning our intent. 
For example, when assigning a variable (which has a previously cached value), 
we actually expect it to be assigned to the new given value, not the cached one.
This is not the default behavior, you have to explicitly state that you want to override the cached value
through a force keyword.
CMake also adds a fair amount of initial complexity to a project, and in our view it does not
seem to scale particularly well, without larger efforts from the programmers using it.
In our case CMake lead to a lot of errors, and felt like a house of cards, which easily came down.
It took us a long time to debug these issues, and often turned into witchcraft programming,
done through a trial and error approach.
CMake is definitely not beginner friendly in our view, however this can also be because this is our
first interaction with CMake and the project was of a non trivial size.

# Process and Communication
For the project we chose a scrum like approach, which was supposed to be relatively low overhead,
as we were two team members.
We had daily scrum meetings to inform each other of the progress that had been made the previous work day,
and the plan for the current day.
These meetings were kept short, and were documented in the standard daily scrum latex format,
in bulletpoint like fashion.
The format can be seen at: https://github.com/Per-Morten/imt3912_thesis/blob/master/formats/daily_scrum_format.tex

The development of the project was organized into several sprints, with each sprint lasting a week.
Planning a sprint was mainly done in a regular txt document, before being turned into issues in our issue tracker.
Each sprint were planned after the conclusion of the previous one, with a smaller undocumented retrospective.
A larger and documented retrospective was done biweekly, and here we evaluated if we needed to re-plan our project.
Stakeholder and supervisor meetings was done mainly when there were questions we needed answers to,
or if we had reached a particular milestone.

Communication was mainly done orally, or through discord. In the discord chat we had several channels for each different
category, allowing us to have a log of previous discussions on certain issues.

In general we tried to keep the process and communication low overhead. We were only two people, who worked relatively
independent of each other. Having a larger more formal process would potentially look better, and might have saved us
some issues, but we are uncertain if the larger overhead would be worth it.

## Timing and Estimation
We wanted to track as much information as possible related to the development of the project.
To do this we used toggl, and added undocument guidelines on the naming of the task.
Each time entry would be named according to the issue we were working with at the time.
When creating issues each issue would be given an estimate on how long we thought it would take to complete it.
The original idea was to create a script that would download and parse the different toggl entries,
and match them with the time estimations in the git issues.
The script would then output the time difference between the estimation and actual time spent.
Having this information would tell us to find out how accurate our estimations were,
and allow us to make better estimations for the future.
Unfortunately we did not have enough time to implement this script properly.
We had one version early in development which would look at the bitbucket issues, but it never supported
github, or sub tasks, and was therefore not used.

## Kanban Board
After moving the repository over to github we got access their project board, which we used
as a simple kanban board.
We created a new board per sprint, and followed a simple todo, in progress, review ready and done scheme.



