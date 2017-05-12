#include "colors.inc"
#include "woods.inc"
#include "pascal.inc"
#include "puzzle.inc"

camera 
{
    up y
    right x*50/32
    location < 0, 4.9, 0 >
//    location < 0, 5, 5 >	// Kijk schuin 
//    location < 0, 0.5, 2 >	// Kijk laag 
    look_at < 0, 0, 0 >
}

#declare PuzzleBody = box {
    <-PuzzleWidth/2,0,-PuzzleHeight/2>
    <PuzzleWidth/2,PuzzleThickness,PuzzleHeight/2>
}

#declare Puzzle = difference {
    union {
	object { PuzzleBody }
	object { PuzzleHead translate <0,0,PuzzleHeight/2> }
	object { PuzzleHead rotate 180*y translate <0,0,-PuzzleHeight/2> }
    }
    object { PuzzleHead scale <1.1,1.1,1.1> rotate y*90 translate <-PuzzleWidth/2,-Eps,0> }
    object { PuzzleHead scale <1.1,1.1,1.1> rotate y*-90 translate <PuzzleWidth/2,-Eps,0> }
    texture { S_Wood1 scale 2 rotate <3,3,0> }
    finish { phong 0 }
}

object { Puzzle rotate y*90 scale 1.35 }
