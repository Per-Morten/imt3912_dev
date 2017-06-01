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
Additionally the language does not have proper scope mechanisms by default, meaning that a lot of variables are reused.
The keyword local can be used to get scoping mechanisms, but not having it enabled by default is a huge pain.
Bash does not allow for proper return values either, meaning that you either have to echo the return value,
or introduce global variables.
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

# Git Strategy
## Commit Style
For the project we wanted a clear git history, with informative commits that reflected the work that was done.
The commit style was inspired by an online post, and the pt commit standard can be seen in the wiki (https://github.com/Per-Morten/imt3912_thesis/wiki/commit_standard).
We tried to follow this style as much as possible, and also added a simple git commit hook to enforce the format of the commit header.
All commits were split into 3 parts, a header, body and footer.
The header was supposed to reflect the issue the commit was tied to, and a one line description of what was done, written in imperative form.
If needed the commit would include a body, explaining more about the commit, this could be written in free form.
A footer with reference to other relevant issues would also be added if needed.

## Issue tracking
In general we tried to always tie commits to issues, to give clear indication to what we were working on,
and what problems we were solving.

### Tools
We went through different tools for tracking issues throughout the project.
Originally we tried using JIRA, but moved over to bitbuckets issue tracker after being unable to comment commits in JIRA.
We used bitbuckets issue tracker for a while, but we were annoyed by the lack of useful features, like grouping issues towards milestones etc.
Githubs issue tracking offered more functionality that we were interested in, like timed milestones.
Moving the issues from one repo to another was relatively straight forward, as we found a simple script online that did it for us.

### Standard
Originally when working with bitbucket we had a standard describing how issues should look, however, this was not followed as much
late in development. This standard can be seen in the wiki: https://github.com/Per-Morten/imt3912_thesis/wiki/issue_standard

### Splitting up Tasks
A challenge when working with issue trackers was to decide how fine-grained the issues should be.
For example, was it an issue to implement the whole event class, or was implementing each function
its own issue. This was also important when estimating and logging time, so we could see which issues we spent the most time on,
and how wrong our estimations were.
Originally we tried to fix this by adding what we called sub tasks, as bitbuckets issue tracker did not natively support this.
A subtask was simply a new task with a reference to the parent task, written as "$sub #<task_number>".
A script would then go through each issue, find its parents and combine the total time spent on each issue.
The concept of subtasks was dropped early-mid development, as it was not working as desired.

We did not find any real solution to the granularity level of specifying issues, but stopped caring after a certain point.

### Priority
When originally working with bitbuckets tasks we would assign priority to the different issues.
However, this was dropped when moving over to githubs issue tracker. Mainly because the feature was not really used
that much. Seeing as we were only two developers it was quicker to just ask the other person to help out
if a certain task had a higher priority.

## Branching
Originally we chose a strict issue branching style, where a new branch was created for each issue, and merged once it was reviewed and fixed.
Throughout development we became more lenient on this strategy, allowing for relevant issues to be grouped together.
Having the branching strategy allowed us to work more isolated from each other, and was generally beneficial.
However, it was a bit annoying when the different branches had dependencies on each other, and merges were needed in several directions.
Alternatively it might have been better to have one branch per developer, or larger features, as the current branching graph is pretty complicated.

## Pull requests
After a task had been solved in a branch it would be merged into master.
To ensure consistency and quality in the codebase each pull request needed to pass a review before it could be pulled in.
When accepted the pull request would be merged in, with a standard message: "Accept pull request #<task_number>".
However, since there are different pull request numbers on bitbucket and github, all the bitbucket pull requests that were copied
over lost their id.

## Merging
We wanted a clear git history, showing all the development in the codebase.
Because of this we decided that all merging should be explicit, therefore git merge commands would be run with --no-ff,
to turn off fast forwarding.
Another idea could perhaps be to do rebasing or allow for fast forward merges, as all the merge commits did clutter
up the git history.

## Summary
In general we think that our git usage worked out quite well for us, especially when looking back at the history of development.
However, towards the end of the project we did stray from our normal procedures as we did not have enough time to do everything according to protocol.

# Style guide
The style guide followed is available in the pyroeis thesis in appendix G (http://hdl.handle.net/11250/216763).
In general we followed the style guide, but deviated from it on some minor points.
The most important ones being namespaces and implementation files,
the other ones being placement of comma in initializer list, and explicit boolean checking.

## Namespaces
Normally in NOX namespaces are declared at the top of the file, and there seems to be inconsistencies
regarding indentation or braces placement on the namespaces, and how different classes are predeclared.
This is the case both for the header files and the implementation files.
One of the reasons for us deviating from existing practice was born out of frustration of not being able to quickly
identify which file or namespace we were in.
We chose to include indentation in the header files when declaring namespaces,
and also to write the full namespace within the implementation file.
Making it obvious which namespace we were operating in.

## Implementation Files
The NOX engine normally places inline and template functions at the bottom of the header files.
We believe that this gives unnecessary implementation details in a file that should really only be an interface description.
Because of this we decided to put inline functions within .ipp files and template functions within .tpp files, which
were included in the header files.
Allowing for a more clean separation of interface and implementation.

## Documentation
We decided early on that the ECS should be clearly documented, and followed the javadoc style flavor used by Suttung.
However, we decided to document more heavily than what Suttung normally does, and mandated that almost every function
should be documented.

## Comments
Suttung does not seem to have a style related to commenting implementations.
We mainly commented code that was not obvious, or where we taking advantage of known factors.
For example updating the component map from the "inside out" in the component collection.
Additionally we commented more within the thread related code, as one might need to know a bit more about the context when reading that code.
This also included more "hacky" code.

# Use of Libraries
We did not make any use of any other libraries than the ones already existing within the NOX Engine,
and even the usage of these were only done to a minor degree.
The reason for this was mainly because we did not want to introduce more dependencies into the NOX Engine.
Additionally the project was supposed to experiment with more language features, and introducing libraries
that did not follow our patterns would not be beneficial for the project.
Potentially we could have reused more of the existing NOX Systems, but they did not follow our development
strategies either.

# Code Review
We did code reviews whenever a pull request needed approval.
The review was done by the other team member, and the main purpose was to enforce consistency in formatting
and quality.
Reviews were done through githubs pull request review features, allowing for comments placed on the various
lines with issues.
Comments on reviews would either have to be fixed, or justified.
Once all issues were fixed and a reviewer was satisfied the code would be merged into its parent,
with a standard commit message linking to the pull request.
In addition to consistency and quality, the motivation behind the reviews was to inform the other member
about what you were doing, a promote understanding of the code you had written.
Our main issue with the pull request reviews was that they quickly became quite large, and with that
the tools of handling pull requests became unwieldy.
Additionally when the pull requests started becoming larger, reviews quickly turned into bikeshedding,
rather than actual productive reviews.

# Professionalism
Following are several points covering other professionalism aspects that we either did,
or wanted to do through the bachelor project.

## Testing
Originally we wanted to do proper unit testing of the different software modules we created,
and follow a more test driven development approach.
We downloaded google test, and was prepared to use it. Unfortunately we did not have enough
time to actually use it, as setting it up required more effort than we could afford to invest.
We did write some ad-hoc tests, mainly within the scratch-pad file main.cpp.
However, these were not kept up to date with interface changes, again because of lack of time.
This is something we want to improve on in the future, as it helps ensuring proper quality and
backwards compatibility.

## Benchmarks
Seeing as we were doing optimization work, proper benchmarks were needed.
Doing optimizations without profiling is not particularly professional,
and is something we should improve on in the future.
The lack of profiling before optimization was mainly based on the time we had available.
Additionally there were some issues with the benchmarks we did not notice until it was to late,
like doing prints etc. This is not particularly good as it could have affected the data.

## Automation
We tried to automate a lot of our processes throughout the bachelor project,
this was meant to save time and also to ensure correctness.
Several scripts like the compile script, and script for creating csv files
allowed us to ensure quality and additionally saving time.

