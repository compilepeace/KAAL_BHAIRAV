# KAAL BHAIRAV
Kaal Bhairav is a **terribly fearful form** of **lord shiva** (**The Destroyer** within the trimurti). Considering the analogy, kaal bhairav (the project) is focussed on exploring the darker side of system programming on linux platform. The software code parses the linux filesystem and infects every piece of file it goes through. All the ELF binaries encountered by it are injected with *parasite code* using an old infection technique called as **segment padding infection** (I encourage you to google it).


##  AIM
This project started out of curiosity so it does'nt encourage destruction (in any context) but is aimed at exploring the limits of system programming and developing a offensive approach towards systems and machines which may result into a better approach towards mitigating modern threat (albeit this is nowhere near to modern threat which leverages a lot more sophisticated techniques of spreading infection and maintaining persistence by obfuscation, encryption, polymorphism and what not). 


## BUILDING
To build the binary, follow the bellow steps ^_~

![clone](./pictures/clone.png)

Now, building the binary !

![build](./pictures/make.png)

Finally running the binary with a \<pathname> as parameter.

![execution](./pictures/execute.png)


**WARNING** - You wouldn't want to run the binary with root priveleges (as a superuser) because any path you provide outside of your home directory may lead to corruption to an extent such that the system may end up being in an unstable state or may become entirely unsable. Also, be careful with the path you provide as argument to kaal bhairav because if the path you provide is a parent or an ancestor just above Kaal bhairav, it may corrupt the project directory too !
<br>
<br>

**NAME**  : Abhinav Thakur *(Mahakaal)* <br>
**EMAIL** : compilepeace@gmail.com  