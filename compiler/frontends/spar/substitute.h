// File: substitute.h

extern TypeDeclaration substitute_TypeDeclaration(
 const_origin org,
 TypeDeclaration decl,
 const_TypeAbstraction_list tl,
 const_ValueAbstraction_list vl
);
extern Block substitute_Block(
 const_origin org,
 Block blk,
 const_TypeAbstraction_list tl,
 const_NameTranslation_list ll,
 const_ValueAbstraction_list vl,
 const_NameTranslation_list sl,
 const_NameTranslation_list nl
);
extern optexpression substitute_optexpression(
 optexpression blk,
 const_ValueAbstraction_list vl,
 const_NameTranslation_list sl,
 const_NameTranslation_list nl
);
