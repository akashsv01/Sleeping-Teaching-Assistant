# Sleeping-Teaching-Assistant
Solution to the Sleeping Teaching Assistant problem in C

➼ **OBJECTIVE**

To solve the process synchronization problem – “Sleeping Teaching Assistant” using semaphores, mutex locks, and threads.

➼ **PROBLEM STATEMENT**

A teaching assistant (TA) helps undergraduate students with their programming assignments during regular office hours. The TA's office is rather small and has room for only one desk with a chair and computer. There are 3 chairs in the hallway outside the office where students can sit and wait if the TA is currently helping another student. When there are no students who need help during office hours, the TA sits at the desk and takes a nap.
If a student arrives during office hours and finds the TA sleeping, the student must wake up the TA to ask for help. If a student arrives and finds the TA currently helping another student, the student sits on one of the chairs in the hallway and waits. If no chairs are available, then the student comes back at a later time.

➼ **ABSTRACT**

The Teaching Assistant has to provide assistance to the under graduate students during office hours with their programming difficulties. The TA office is just a single room with a table and chair. It is not big enough to even accommodate two students at a time. While the TA is busy assisting a student inside office, the students were allowed to wait in the waiting area outside the office which accommodates three chairs. If a chair is empty, the student may occupy it and wait for his/her turn. However, if none of the chair is empty (i.e.) being occupied by some three students, the student decides to come back later. Each time, after assisting a student, TA checks with the students in the waiting area and calls them inside one by one to assist. TA will take a nap if he/she is left with no student in the waiting area. If incase a student arrives when TA is taking a nap, the student may wake up the TA asking for assistance.

➼ **DESCRIPTION**

Three techniques were used to address this problem namely, a) semaphores b) mutual exclusion locks c) threads. We will be capturing two events for which two semaphores are used. One semaphore for the TA, and the other semaphore for students. We initialize 1 thread for TA, and N threads for N students. While the TA assists a student, other students are blocked and when the TA is done with assisting a student, TA will give preference to the student who is waiting for maximum time amongst all to assist next. If incase there were too many students to assist and the chairs in the waiting area were occupied, the student who has just entered the waiting room will leave and come at a later time to ask for TA’s help. On the other hand, TA will take a nap when there are no students to assist.

The TA has three actions. To check for a student, sleep if no one's waiting and wake up when a student arrives, we run a wait() operation on the Student_Sem semaphore. When a student arrives, the TA starts assisting him/her, and then there is one less student in the waiting room. After the tutoring, the TA checks for a student and the cycle repeats.
The student also has three actions but, unlike the TA, the student only performs his/her actions once. When a student arrives, he/she checks to see if there's a chair available in the waiting room. If there isn't, the student leaves. Otherwise the student waits, and the number of students in the waiting room increases. The modification of the number of chairs is protected using mutex lock and unlock mechanisms.





