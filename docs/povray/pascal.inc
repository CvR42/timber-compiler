//
// Textures for 'cristals.pov'
//
// Skal 95 (Pascal.Massimino@ens.fr).
//
// uses the image-maps 'mur.gif', 'mur3.gif' and
// 'vase.gif'...
//
////////////////////////////////////////////////////

#version 2.0
/*
     Textures:
     ---------

 - Pierre1:  Blue/green stone 
 - Pierre2:  Peach/Orange reflective stone      
 - Pierre3:
 - Pierre4:
 - Pierre5:
 - Pierre6:
 - Pierre7:
 - Pierre8:
 - Pierre9:
 - Pierre10:  Dark gray granite with light veins
 - Pierre11:  Green/blue granite
 - Pierre12:  Green/Gray marble
 - Pierre13:  Blue marble with yellow veins
 - Pierre14:  Peach marble
 - Pierre15:  Dark gray with blue veins

 - Granite1:
 - Granite2:  Dark gray granite
 - Granite3:  Dark gray with light veins
 - Granite4:  
 - Granite5:  
 - Granite6:  Black granite with white stains 
 - Granite7:  Blue granite  
 - Granite8:  Light gray granite 
 - Granite9:  Yellow/Orange granite
 - Granite10: Dark granite with light blue cracks

 - Veins1:    Light gray veins    
 - Veins2:    Gray cracks
 - Veins3:    Gray cracks 
 - Veins4:    Dark gray cracks
 - Veins5:    Dark red cracks 
 - Veins6:    Light blue cracks

 - S_Glass:   Not well polished glass
 - S_Glass2:  Clear glass
 - S_Quartz:  Smoked quartz
 - Red_Gem:   Like quartz, but red...
 - S_Wood1:   Light wood
 - S_Wood2:   Dark wood
 - S_Wood3:   Dark wood with lighter stains
 - S_Wood4:
 - S_Wood5:

 - Painting1:
 - Parafin:
 - Flamme_Text:
 - Flare_Text:
 - Candelar_Text:
 - Book_Text:
 - Page_Text:
 - Black_Sky:
 - Shiny_Gold:
 - Cloth_Text:

*/

/******************** misc pigments ********************/

#declare Agate1 =
pigment {
  agate
  color_map {
    [0.10  color rgb <0.52 0.78 0.52 > ]
    [0.20  color rgb <0.52 0.52 0.40 > ]
    [0.30  color rgb <0.27 0.40 0.02 > ]
    [0.40  color rgb <0.27 0.65 0.52 > ]
  }
  agate_turb 0.3
}

#declare Agate2 = 
pigment {
  agate
  color_map {
    [0.10  color rgb < 0.78 0.78 0.78 > ]
    [0.60  color rgb < 0.89 0.52 0.27 > ]
    [0.60  color rgb < 0.60 0.47 0.32 > ]
    [0.68  color rgb < 0.27 0.27 0.30 > ]
    [0.72  color rgb < 0.56 0.40 0.44 > ]
    [1.00  color rgb < 0.52 0.52 0.56 > ]
  }
  agate_turb 0.80
  omega 0.6
  lambda 7
}

#declare Onion1 =
pigment {
   onion
   color_map {
    [0.10  color rgb < 0.40 0.40 0.65 > ]
    [0.40  color rgb < 0.40 0.52 0.40 > ]
    [0.50  color rgb < 0.27 0.40 0.78 > ]
    [1.00  color rgb < 0.52 0.52 1.00 > ]
   }
   turbulence 0.80
   omega 0.80
   lambda  3
}

#declare Bozo1 =
pigment {
   bozo
   color_map {
     [0.00  color rgbf < 0.30 0.95 0.50 0.50 > ]
     [0.20  color rgbf < 0.20 0.78 0.30 0.65 > ]
     [0.80  color rgbf < 0.18 0.62 0.10 0.70 > ]
   }
   turbulence 0.20    // 0.50
   frequency 5.0
}



/*********************** Granites ***************************/

#declare Granite1 =
pigment {
  granite
  color_map {
    [0.00  color rgb < 0.70 0.60 0.30 > ]
    [0.20  color rgb < 0.40 0.40 0.40 > ]
    [0.90  color rgb < 0.27 0.27 0.27 > ]
    [1.00  color rgb < 0.05 0.05 0.05 > ]
  }               
  scale 0.06
}

#declare Granite2 =
pigment {
  granite
  color_map {
    [0.00  color rgb < 0.25 0.25 0.25 > ]
    [0.80  color rgb < 0.27 0.27 0.27 > ]
    [0.90  color rgb < 0.45 0.45 0.45 > ]
  }               
  scale 0.01 
}

// Dark gray w/ light vein granite
#declare Granite3 =
pigment {
   granite
   color_map {
      [0.00  color rgb < 0.13, 0.15, 0.18 > ]
      [0.80  color rgb < 0.25, 0.28, 0.20 > ]
      [0.85  color rgb < 0.15, 0.20, 0.22 > ]
   }
   rotate < 0 , 0 , -60 >
   scale < .6 , 2 , .5 >
}

#declare Granite4 =
pigment {
  marble
  color_map {
    [0.10  color rgb < 0.52 0.52 0.52 > ]
    [1.00  color rgb < 0.52 0.40 0.78 > ]
  }
  turbulence 0.80
  omega 0.80
  lambda  8
  scale < 1 , 3 , 2 >
  rotate < 10 , 0 , 40 >
}

#declare Granite5 =
pigment {
  granite
  color_map {
    [ .00  color rgb < 0.40 0.52 0.45 > ]
    [ .70  color rgb < 0.32 0.52 0.62 > ]
    [ .75  color rgb < 0.32 0.30 0.28 > ]
    [ .95  color rgb < 0.42 0.50 0.58 > ]
  }
  scale < 3 , 3 , 4 >
  scale 0.08
}

// Black granite with white stains
#declare Granite6 =
pigment {
  granite
  color_map {
    [ .00  color rgb < 0.12 0.12 0.12 > ]
    [ .90  color rgb < 0.12 0.22 0.12 > ]
    [ .95  color rgb < 0.45 0.58 0.48 > ]
  }
  scale < 2 , 4 , 2 >
  rotate < 30 , 0 , 50 >
}

// Blue granite
#declare Granite7 =
pigment {
  granite
  color_map {
    [ 0.000  color rgb < 0.098  0.098  0.196 > ]
    [ 0.228  color rgb < 0.255  0.255  0.349 > ] 
    [ 0.325  color rgb < 0.302  0.302  0.416 > ] 
    [ 0.447  color rgb < 0.255  0.255  0.349 > ]
    [ 0.535  color rgb < 0.149  0.149  0.259 > ]
    [ 0.623  color rgb < 0.094  0.094  0.196 > ]
    [ 0.816  color rgb < 0.294  0.294  0.408 > ]
    [ 1.000  color rgb < 0.098  0.098  0.196 > ]
  }
  scale < 2 , 4 , 2 >
  rotate < 0 , 0 , -40 >
}

// Light granite
#declare Granite8 =
pigment {
    granite
    color_map {
      [0.00  color rgb < 0.63, 0.61, 0.65 > ]
      [0.70  color rgb < 0.55, 0.58, 0.63 > ]
      [0.85  color rgb < 0.23, 0.21, 0.28 > ]
    }
    scale < 2.5 , .8 , .7 > 
    rotate < 0 , 0 , -45 >
}

#declare Granite9 =
pigment {
    bozo
    turbulence 0.5
    octaves 4
    lambda 6
    omega .55
    color_map {
      [ 0.00 color rgb < 0.235  0.035  0.027 > ]
      [ 0.12 color rgb < 0.545  0.255  0.184 > ]
      [ 0.43 color rgb < 0.816  0.537  0.027 > ]
      [ 0.63 color rgb < 1.000  1.000  0.588 > ]
      [ 0.91 color rgb < 0.839  0.604  0.071 > ]
    }
    scale < 3.8 , 1.8 , 2.1 >
    rotate < -25 , 0 , 0 > 
}

// Dark granite with light blue cracks
#declare Granite10 =
pigment {
   granite
   color_map {
       [ 0.00 color rgb < 0.300  0.645  0.715 > ]
       [ 0.35 color rgb < 0.239  0.235  0.239 > ]
       [ 1.00 color rgb < 0.239  0.235  0.239 > ]
   }
   scale < 3.1 4.8 2.8 >
   rotate < 0 , 0 , -30 > 
}

/******************** Veins & Cracks ***************/

// Light gray veins 
#declare Veins1 =
pigment {
   granite
   turbulence 0.40
   color_map {
      [ 0.00 color rgb < 0.788 , 0.761 , 0.676 > ]
      [ 0.15 color rgbf < 0.788 , 0.761 , 0.676 , .70 > ]
      [ 0.30 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
      [ 1.00 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
   }
  scale < .8 , 1.6 , .8 >
  rotate < 0 , 0 , 30 >
}

// Gray cracks
#declare Veins2 =
pigment {
   granite
   color_map {
      [ 0.00 color rgb < .40 , .48 , .42 > ]
      [ 0.30 color rgbf < 1.0 , 1.0 , 1.0 , .70 > ]
      [ 1.00 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
   }
   scale < 1.5 , 2.2 , 1.4 >
   rotate < -30 , 0 , 50 >

}

// Gray cracks
#declare Veins3 =
pigment {
  granite
  color_map {
      [ 0.00 color rgb < 0.55, 0.60, 0.62 > ]
      [ 0.10 color rgbf < 1.0 , 1.0 , 1.0 , .70 > ]
      [ 1.00 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
  }
  scale < 1.5, 4.4 , 1.4 >
  rotate < -30 , 0 , 50 >
}

// Dark gray cracks
#declare Veins4 =
pigment {
   granite
   color_map {
      [ 0.00 color rgb < 0.17, 0.29, 0.28 > ]
      [ 0.10 color rgbf < 1.0 , 1.0 , 1.0 , .80 > ]
      [ 0.40 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
      [ 0.65 color rgbf < 1.0 , 1.0 , 1.0 , .60 > ]
      [ 0.73 color rgb < .16 , .20 , .28 > ] 
      [ 0.80 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
   }
   scale < 3.5 , 2.8 , 3.2 >
   rotate < 0 , 0 , 40 >
}

// Dark red cracks
#declare Veins5 =
pigment {
   granite
   color_map {
      [ 0.00 color rgbf < 0.235  0.035  0.027 0.0 > ]
      [ 0.19 color rgbf < 0.465  0.275  0.184 .40 > ]
      [ 0.25 color rgbf < 0.716  0.537  0.027 .70 > ]
      [ 0.65 color rgbf < 1.0 1.0 1.0 1.0 > ]
      [ 1.00 color rgbf < 1.0 1.0 1.0 1.0 > ]
   }
   scale < 4.2 2.2 2.3 >
   rotate < 25 , 0 , 0 >
}

// Light blue cracks
#declare Veins6 =
pigment {
  granite
  color_map {
     [ 0.00 color rgbf < 0.452  0.940  0.960 .1 > ]
     [ 0.20 color rgbf < 0.165  0.745  0.867 .8 > ]
     [ 0.30 color rgbf < 1.0  1.0  1.0  1.0 > ]
     [ 0.60 color rgbf < 1.0  1.0  1.0  1.0 > ]
     [ 0.85 color rgbf < .33  .33  .33 .40 > ]
     [ 1.00 color rgbf < 1.0  1.0  1.0  1.0 > ]
  }
  scale < 2.4 4.8 1.8 >
  rotate < 0 , 0 , 50 >
}

/******************* Final stones ***********************/

// Blue/green stone

#declare Pierre1 =
texture {
 pigment { Agate1 }
 finish {
   diffuse 0.60
   phong 0.20
   phong_size  5
 }
}


// Peach/Orange reflective stone
#declare Pierre2 =
texture {
 pigment { Agate2 }
 normal {
   bumps 0.30
   scale 0.2
   turbulence 0.40
 }
 finish {
  diffuse 0.40
  ambient 0.20
  reflection 0.30
 }
 scale 0.8
}


// Sparkling blue stone
#declare Pierre3 =
texture {
 pigment { Onion1 }
 normal {
   dents 0.80
   scale 0.05
   turbulence 0.70
 }
 finish {
  diffuse 0.60
  ambient 0.20
  reflection 0.3
  phong 0.20
  phong_size 2.0
  specular 0.10
  roughness 0.08
 }
 scale 0.2
}


// Gray stone
#declare Pierre4 =
texture {
 pigment { Granite4 }
 normal {
   wrinkles 0.40
   scale 0.08
   turbulence 0.80
 }
 finish {
  diffuse 0.60
  reflection 0.10
  phong 0.80
  phong_size  2
 }
}



// fluorin stone
#declare Pierre5 =
texture {
 pigment { Bozo1 }
 /*
 normal {
   ripples 0.30
   scale <2.5,2.5,3.0>
   frequency  6
   turbulence 0.2
 }
 */
 finish {
  diffuse 0.40
  ambient 0.10
  reflection 0.40
  refraction 1.0
  phong 0.80
  phong_size 5   // 60
  specular 0.20
 }
}


#declare Pierre8 =
texture {
 pigment { Granite1 }
 finish {
  diffuse 0.50  // 0.40
  ambient 0.20
  reflection 0.0
  phong 0.30
  phong_size  30    // 15
 }
 normal {
   bumps .6
   scale 0.05   
 }
}


#declare Pierre9 =
texture {
  pigment { Granite8 }
  finish {
    ambient .2
    diffuse .4
    reflection 0.0
  }
}
texture { 
 pigment { Veins4 }
 finish {
    phong .4
    phong_size 30
 }
 normal {
   bumps .3
   scale .2
 }
}

#declare Pierre10 =
texture {
 pigment { Granite3 }
 finish {
    ambient .2
    diffuse .4
    reflection 0.0
 }
}
texture { 
 pigment { Veins3 }
 finish {
    phong .4
    phong_size 30
 }
}

#declare Pierre11 =
texture {
 pigment { Granite5 }
 finish {
   ambient .2
   diffuse .7
   reflection 0.2
   phong .6
   phong_size 120
 }
 normal {
   bumps .05
   scale .012
 }
}

// Green/Gray marble
#declare Pierre12 =
texture {
  pigment { Granite6 }
  finish { ambient .20 diffuse .90 }
}
texture {
  pigment { Veins2 }
  scale .8       
  finish { phong .9 phong_size 90 }
  normal {
   wrinkles .3
   scale .2
  }
}

// Blue marble-like stone
#declare Pierre13 =
texture {
  pigment { Granite7 }
  finish { ambient .20 diffuse .90 }
}
texture {
  pigment { Veins1 }
  finish { phong .9 phong_size 90 }
}

// Yellow/peach marble with dark veins
#declare Pierre14 =
texture {
  pigment { Granite9 }
  finish { ambient .20 diffuse .90 }
}
texture {
  pigment { Veins5 }
  finish { ambient .20 diffuse .90 phong .9 phong_size 90 }
}

// Dark marble with light blue veins
#declare Pierre15 =
texture {
  pigment { Granite10 }
  finish { ambient .20 diffuse .90 }
}
texture {
  pigment { Veins6 }
  finish { ambient .20 diffuse .60 phong .9 phong_size 90 }
}

/*******************************************************/

#declare S_Glass = 
texture {
  pigment { color rgbf < 0.90, 0.90, 0.95, 0.9 > }
  normal { bumps .2 scale < .1, .1 , 5 > rotate < 45 , 0 , 0 > }
  finish {
     ambient 0.1
     diffuse 0.2
     reflection 0.15
     refraction 0.8
     ior 1.1
     specular 0.8     
     roughness 0.06
     phong 1 phong_size 50
  }
}

#declare S_Glass2 = 
texture {
  pigment { color rgbf < 0.92, 0.95, 0.96, 0.98 > }
  finish {
     ambient 0.24
     diffuse 0.21
     reflection 0.15
     refraction 0.88
     ior 1.3
     specular 0.7     
     roughness 0.005
     phong .4 phong_size 150
  }
}

#declare S_Quartz = 
texture
{
    pigment { color rgbf < 0.90, 0.90, 0.55, 0.9 > }
    finish {
      ambient 0.1
      diffuse 0.1
      reflection 0.2
      refraction 0.8
      ior 1.4
      specular 0.8     // .8                       
      roughness 0.04
      phong 1 phong_size 400
    }
}

#declare Red_Gem = 
texture
{
    pigment { color rgbf < 0.90, 0.55, 0.55, 0.9 > }
    finish {
      ambient 0.1
      diffuse 0.1
      reflection 0.2
      refraction 0.8
      ior 1.4
      specular 0.8     // .8                       
      roughness 0.04
      phong 1 phong_size 400
    }
}


/*********************** Woods ***************************/
// X is the main axis

#declare Wood_Pigment1 = 
pigment {
   wood
   turbulence 0.09
   octaves 3
   color_map { 
     [ 0.00  color rgb < 0.89 , 0.60 , 0.30 > ]
     [ 0.10  color rgb < 0.89 , 0.60 , 0.30 > ]
     [ 0.90  color rgb < 0.60 , 0.40 , 0.20 > ]
     [ 1.00  color rgb < 0.60 , 0.40 , 0.20 > ]
   }
   scale < 0.2 , 0.2 , 1.1 > 
   rotate < 2 , 90 , 1 >
}

#declare Wood_Pigment2 =
pigment {
   wood
   turbulence 0.08
   octaves 4
   lambda 0.60
   
   color_map {
      [ 0.00  color rgb < 0.27 0.22 0.00 > ]
      [ 0.80  color rgb < 0.35 0.24 0.13 > ]
      [ 1.00  color rgb < 0.65 0.40 0.14 > ]
   }
   scale < 0.2 , 0.2 , 1 > 
   rotate < -1 , 90 , 1 >
}

#declare Wood_Pigment3 = 
pigment {
   wood
   turbulence 0.08
   octaves 4
   lambda 0.60
   color_map {
      [0.10  color rgb < 0.27 0.02 0.00 > ]
      [0.80  color rgb < 0.35 0.14 0.13 > ]
      [1.00  color rgb < 0.65 0.40 0.14 > ]
   }
   scale < 0.2 , 0.2 , 1 > 
   rotate < -1 , 90 , 2 >
}

#declare Wood_Pigment4 = 
pigment {
   wood
   turbulence 0.06
   octaves 4
   lambda 0.60
   color_map {
     [ 0.000  color rgb < 0.561  0.416  0.282 > ]   
     [ 0.050  color rgb < 0.788  0.588  0.369 > ]
     [ 0.983  color rgb < 0.647  0.502  0.396 > ]
   }
   scale < 0.2 , 0.2 , 1 >
   rotate < -1 , 90 , 0 > 
}

#declare Wood_Pigment5 =
pigment {
   wood
   turbulence 0.09
   octaves 4
   lambda 0.60
   color_map {
     [ 0.000  color rgb < 0.561  0.416  0.282 > ]   
     [ 0.050  color rgb < 0.788  0.588  0.369 > ]
     [ 0.983  color rgb < 0.647  0.502  0.396 > ]
   }
   scale < 0.2 , 0.2 , 1 >
   rotate < 2 , 90 , 2 >
}

#declare Wood_Veins1 =
pigment {
  granite
  color_map {
     [ 0.000 color rgbf < 0.204  0.055  0.000 0.0 > ]
     [ 0.059 color rgbf < 0.412  0.227  0.020 0.20 > ]
     [ 0.378 color rgbf < 0.667  0.467  0.325 0.88 > ]
     [ 0.748 color rgbf < 0.208  0.212  0.200 0.92 > ]
     [ 1.000 color rgbf < 0.204  0.055  0.000 0.95 > ]
  }
  scale < 1.4 1.4 1.6 >
  rotate < 0 , 20 , 50 >
}

/****************** Final woods *********************/

#declare S_Wood1 = 
texture {
 pigment { Wood_Pigment1 }
 finish {
   ambient .30
   diffuse .50
   reflection 0.0
   specular .10
   roughness 0.002
   phong .40
   phong_size 16
  }
}

#declare S_Wood2 = 
texture {
 pigment { Wood_Pigment2 }
 finish {
   ambient 0.05
   diffuse 0.40
   reflection 0.05
   specular 0.20
   roughness .04
   phong 0.30
   phong_size 20
   brilliance 2
 }
}

#declare S_Wood3 = 
texture {
 pigment { Wood_Pigment3 }
 finish {
   ambient 0.05
   diffuse 0.40
   specular 0.10
   roughness .4
 }
}

#declare S_Wood4 =
texture {
 pigment { Wood_Pigment4 }
 finish {
   ambient 0.05
   diffuse 0.40
   reflection 0.0
   specular 0.10
   roughness .4
 }
}

#declare S_Wood5 =
texture {
  pigment { Wood_Pigment5 }
  finish {
     ambient 0.05
     diffuse 0.40
     reflection 0.2
     specular 0.10
     roughness .4
  }
}
texture {
  pigment { Wood_Veins1 }
  finish { ambient .20 diffuse .60 phong .3 phong_size 45 }
}

/*********************************************************/


/*****************************************************/

#declare Parafin =
texture {
  pigment { color rgb < 0.85, 0.1 , 0.1 > }
  finish {
    ambient 0.30
    diffuse 0.60
    phong 0.30
    phong_size 2
    brilliance 2
  }
  normal { 
    bumps .5
    scale < 0.05 , 0.05 , .3 >
  }
}

#declare Flamme_Text = 
texture {
  pigment { 
   gradient < 0.0, 0.0, 1 >
   color_map { 
     [ 0.0   color rgbf < 0.8, 0.6, 0.4, .6 > ]
     [ 0.5   color rgbf < .96 , 0.75 , 0.33, 0.0>  ] 
   }
  }
  finish {
    ambient 1.0
    diffuse 0.0
    reflection 0.0
  }
  turbulence .4
  scale < 1, 1, 5>
}

#declare Flare_Text =
texture {
  pigment { color rgbf < 1.0, 1.0, 1.0, 1.0 > }
  finish {
    ambient 0.0
    reflection 0.0
    refraction 1.0
    // diffuse 0.2
    specular .8
    roughness 0.025 // 0.02
  }
}

#declare Candelar_Text =         
texture {
  pigment { color rgb < 1.0 , 0.80 , 0.46 > }
  finish {
    ambient 0.11
    diffuse 0.43
    brilliance 2
    reflection 0.3
    specular 0.2
    roughness 0.02
    phong 0.53
    phong_size 5
    metallic
  }
  normal {
    dents .9
    scale 0.03
  }
}


/******************************************************/
#declare Book_Text =
texture {
   pigment {
      marble
      color_map {
         [ .00 color rgb < .00 , 0.25 , .25 > ]
         [ .90 color rgb < .00 , 0.25 , .25 > ] 
         [ .00 color rgb < .40 , 0.30 , .10 > ]
      }
      turbulence .6
      scale 0.12
   }
   normal {
      bumps 0.6  //  .9
      turbulence .6
      scale 0.05        // 0.03
   }
   finish {
     ambient .1
     diffuse .3
     reflection 0.0 // 0.04
     phong .3
     phong_size 8
   }
}

#declare Page_Text =
texture {
  pigment {
     bozo
     color_map {
         [ .00 color rgb < .90 , 0.90 , .80 > ]
         [ .90 color rgb < .80 , 0.70 , .40 > ]    
         [ .90 color rgb < .55 , 0.50 , .40 > ] 
     }
     turbulence .05
     scale 0.05
     scale < 32 , 32 , .78 >
  }
  finish {
    ambient .15
    diffuse .4
  }
}

#declare Black_Sky = 
texture {
 pigment { 
   bozo
   turbulence 0.4
   color_map { 
     [ 0.00  color rgb < 0.00 , 0.00 , 0.00 > ]
     [ 0.65  color rgb < 0.02 , 0.02 , 0.05 > ]
     [ 0.80  color rgb < 0.10 , 0.10 , 0.10 > ]
     [ 1.00  color rgb < 0.03 , 0.03 , 0.03 > ]
   }
   scale < 1 , 6 , 6 >
 }
 finish {
    ambient 1.0
    diffuse 0.0
 }
 normal { bumps .2 scale .3 }
}
texture {
  pigment {
    gradient x
    color_map {
      [ 0.0  color rgbf < 0.1 , 0.1 , 0.1 , 0.5 > ]
      [ 0.5  color rgbf < .6 , .6 , .6 , 1.0 > ]
    }
    turbulence .5
    scale 8
  }
  finish {
    ambient 1.0
    diffuse 0.0
  }
}

/*********************************************************/

#declare Shiny_Gold = 
texture {
 pigment { color rgb < 0.90 , 0.68 , 0.3 > }
 finish {
   metallic
   ambient 0.2
   diffuse 0.6
   brilliance 4
   reflection 0.25
   phong 0.75
   phong_size 80
 }
}


#declare Cloth_Pigment =
pigment {
  marble
  color_map {
        [ 0.00 color rgb < .90 , .68 , .40 > ]
        [ 0.30 color rgb < .90 , .50 , .50 > ]
        [ 0.30 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
        [ 1.00 color rgbf < 1.0 , 1.0 , 1.0 , 1.0 > ]
  }
  turbulence .2
  scale < .1 , .1 , 5 >
}

#declare Cloth_Text = 
texture {
 pigment { color rgb < .90 , .30 , .20 > }
 finish {
   ambient 0.2
   diffuse 0.6
   phong .40
   phong_size 15
 }
}
texture {
 pigment { Cloth_Pigment rotate < 45 , 0 , 0 > }
 normal {
   bumps .6
   scale .05
 }
}
texture {
  pigment { Cloth_Pigment rotate < -45 , 0 , 0 > }
  normal {
   bumps .6
   scale .05
  }
}


///////////////////////////////////////////////////////////////////
