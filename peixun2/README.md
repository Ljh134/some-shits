this program mix opencv and ros to detect the armoured plate devided in 3 packages all of them are in SRC

msg_pkg loads the message tube of ros
peixun2_pkg act as the main processing and message publisher
subscriber_pkg is the subscriber and match and pair the lights to detect the armnoured plate

specificly:

1 every package have a CMakeLists.txt and a .xml (created autonomously created by ros but should be polish )  REMEMBER FOLLOW:

NEVER NEVER NEVER try to understand what is written in it , just use it by copy and paste ( just change every pkg's name to your pkg's name )
they are just pieces of shit and have no any logic , but i will note it that when you should paste this part

2 if you want to specificly build the packages , please bulid the msg_pkg first by using terminal : colcon build --packages-select msg_pkg

3 in peixun2_pkg the core is the pro gram "advance" read that before read this 

4 bash can only start running peixun2_pkg and subscriber_pkg , terminal: colcon build    first
(bash 1 for peixun 2 , bash 2 for subscriber)
running the bash2 first in the first terminal then run bash 1 in another terminal


5 in msg_pkg I want to send some 3-dimensioned arrey thus using nestd messages if you want to find ordinary message , turn to project "my_pkg"


6 if cmake err of "you have change the place of CMakeLists.txt ....... couldn't cmake" happened , or you want to move the project (even though move the entired file ) please delete every filebox without "SRC" and then colcon build again (this is capable for every situation like this )


7 don't change the order of lines in .xml or CMakeLists.txt easily to prevent happening some mistrial bugs