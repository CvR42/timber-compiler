/* ctests2.cc
 */

#include "expr.h"
#include "view.h"

VnusInt numberOfProcessors = 1;

IntegerShape A(1,1,20);
IntegerView Q(1);
IntegerView V(1);
IntegerView W(1);

main()
{
    Expr* vrot = new BinaryExpr(addOp,
                                new BinaryExpr(mulOp, new ConstExpr(3),
                                               new ConstExpr(5)),
                                new BinaryExpr(mulOp, new VarExpr(1),
                                               new VarExpr(0)));
    intVec dp;
    ExprVec ip;
    ip.resize(2);
    dp.resize(2);
    dp[0]=5;
    dp[1]=7;
    ip[0] = new BinaryExpr(divOp, new ConstExpr(8), new ConstExpr(2));
    ip[1] = new BinaryExpr(subOp, new ConstExpr(32), new ConstExpr(4));
    cout << ip[0] << "\n";
    cout << ip[1] << "\n";
    cout << vrot << "\n";
    cout << vrot->Eval(dp) << "\n";
    vrot->Simplify(vrot);
    cout << vrot << "\n";
    cout << vrot->Eval(dp) << "\n";
    vrot->Substitute(vrot, ip);
    cout << vrot << "\n";    
    cout << vrot->Eval(dp) << "\n";
    vrot->Simplify(vrot);
    cout << vrot << "\n";
    ip.resize(1);
    dp.resize(1);
    ip[0] = new VarExpr(0);
    dp[0] = 20;
    Q.AssignView(&A,ip,dp);
    ip.resize(1);
    dp.resize(1);
    ip[0] = new BinaryExpr(mulOp, new ConstExpr(2), new VarExpr(0));
    dp[0] = 10;
    V.AssignView(&A,ip,dp);
    ip[0] = new BinaryExpr(mulOp, new ConstExpr(3), new VarExpr(0));
    dp[0] = 4;
    W.AssignView(&V,ip,dp);
    for (int i=0; i<20; i++)
        A(i)=111;
    cout << A << "\n";
    cout << Q << "\n";
    cout << V << "\n";
    cout << W << "\n";
    for (int i=0; i<10; i++)
        V(i)=222;
    cout << A << "\n";
    cout << V << "\n";
    cout << W << "\n";
    for (int i=0; i<4; i++)
        W(i)=333;
    cout << A << "\n";
    cout << V << "\n";
    cout << W << "\n";
}
