to compile

mkdir -p out/production/visualizer/
javac -g -sourcepath src -d out/production/visualizer/ `find src -iname
*.java`



to run:

java -ea -cp out/production/visualizer DisplayWorld ../../omni/contest10.map


cat ../maps/contest1.map | java -ea -cp out/production/visualizer DasaConsole



rm -f *.class
gcj -fsource=1.6 -w -C AStar.java  Cell.java  DasaConsole.java  DualAStarApproach.java  RobotAction.java  SolidWorldState.java  StepLogic.java  StepResult.java  WorldStateHash.java  WorldState.java
gcj -static-libgcj -o DasaConsole --main=DasaConsole -save-temps *.class

