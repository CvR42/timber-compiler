#include "colors.inc"
#include "woods.inc"
#include "pascal.inc"

global_settings { assumed_gamma 1 }

#declare ioTexture = texture {
    pigment { color Red }
    finish { phong 1 metallic }
}

#declare nameTexture = texture {
    pigment { color Black }
    finish { phong 5 metallic }
}

#declare ioFont = "cyrvetic.ttf"
#declare nameFont = "timrom.ttf"

#declare Eps = 0.001

#declare PuzzleThickness = 0.2
#declare PuzzleWidth = 3.0
#declare PuzzleHeight = 3.0

#declare PuzzleHeadWidth = 0.80
#declare PuzzleHeadHeight = 0.75*PuzzleHeadWidth
#declare PuzzleHeadTargetSeparation = 1.5*PuzzleHeadHeight
#declare PuzzleHeadShift = 0.95*PuzzleHeadHeight

#declare LetterThickness = 0.07

#declare PuzzleHeadOval = cylinder {
    <0,0,0> <0,1,0> 1
    scale <PuzzleHeadWidth,PuzzleThickness,PuzzleHeadHeight>
}

#declare PuzzleHeadBlobs = blob {
    threshold 0.65
    cylinder {
	<0,0,0> <0,10,0> 1 2
	scale <PuzzleHeadWidth,PuzzleThickness,PuzzleHeadHeight>
    }
    cylinder {
	<0,0,0> <0,10,0> 1 3
	scale <PuzzleHeadWidth,PuzzleThickness,PuzzleHeadHeight>
	translate <0,0,-PuzzleHeadTargetSeparation>
    }
}

#declare PuzzleHead = intersection {
    object { PuzzleHeadBlobs }
    box { <-100,0,-PuzzleHeadShift-Eps> <100,PuzzleThickness,100> }
    translate <0,0,PuzzleHeadShift>
}

plane {
     <0, 1, 0>, 0
    pigment { color White }
    finish { ambient 0.6 }
}

light_source {
    <-5, 10, 5>
    color rgb < 1 1 1 >
}

