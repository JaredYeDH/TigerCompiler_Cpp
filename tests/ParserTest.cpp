#include "gtest/gtest.h"

#include "Parser.h"
#include <typeinfo>
#include <iostream>

using namespace std;
using namespace AST;

unique_ptr<Program> MakeTestProgram(unique_ptr<Expression>&& expr)
{
    return make_unique<Program>(move(expr), make_shared<CompileTimeErrorReporter>(), make_shared<WarningReporter>());
}

void AssertThatAstsMatch(const Expression& lhs, const Expression& rhs);

void AssertThatTypesMatch(const TypeNode& lhs, const TypeNode& rhs)
{
    const char* lTy = typeid(lhs).name();
    const char* rTy = typeid(rhs).name();

    if (strcmp(lTy, rTy) != 0)
    {
        cout << "lTy: " << lTy << "\nrTy: " << rTy << "\n";
        ASSERT_EQ(true, false);
    }

    const char* namTy = typeid(NameType).name();
    const char* recTy = typeid(RecordType).name();
    const char* arrTy = typeid(ArrayType).name();

    if (strcmp(lTy, namTy) == 0)
    {
        auto lv = dynamic_cast<const NameType*>(&lhs);
        auto rv = dynamic_cast<const NameType*>(&rhs);
        ASSERT_EQ(lv->name, rv->name);
    }
    if (strcmp(lTy, recTy) == 0)
    {
        auto lv = dynamic_cast<const RecordType*>(&lhs);
        auto rv = dynamic_cast<const RecordType*>(&rhs);
        for (unsigned int i = 0; i < lv->fields.size(); ++i)
        {
            ASSERT_EQ(lv->fields[i].name, rv->fields[i].name);
            ASSERT_EQ(lv->fields[i].type, rv->fields[i].type);
        }
    }
    if (strcmp(lTy, arrTy) == 0)
    {
        auto lv = dynamic_cast<const ArrayType*>(&lhs);
        auto rv = dynamic_cast<const ArrayType*>(&rhs);
        ASSERT_EQ(lv->name, rv->name);
    }
}

void AssertThatVarsMatch(const Var& lhs, const Var& rhs)
{
    const char* lTy = typeid(lhs).name();
    const char* rTy = typeid(rhs).name();

    if (strcmp(lTy, rTy) != 0)
    {
        cout << "lTy: " << lTy << "\nrTy: " << rTy << "\n";
        ASSERT_EQ(true, false);
    }

    const char* smpTy = typeid(SimpleVar).name();
    const char* fldTy = typeid(FieldVar).name();
    const char* subTy = typeid(SubscriptVar).name();

    if (strcmp(lTy, smpTy) == 0)
    {
        auto lv = dynamic_cast<const SimpleVar*>(&lhs);
        auto rv = dynamic_cast<const SimpleVar*>(&rhs);
        ASSERT_EQ(lv->symbol, rv->symbol);
    }
    if (strcmp(lTy, fldTy) == 0)
    {
        auto lv = dynamic_cast<const FieldVar*>(&lhs);
        auto rv = dynamic_cast<const FieldVar*>(&rhs);
        ASSERT_EQ(lv->symbol, rv->symbol);
        AssertThatVarsMatch(*lv->var, *rv->var);
    }
    if (strcmp(lTy, subTy) == 0)
    {
        auto lv = dynamic_cast<const SubscriptVar*>(&lhs);
        auto rv = dynamic_cast<const SubscriptVar*>(&rhs);
        AssertThatVarsMatch(*lv->var, *rv->var);
        AssertThatAstsMatch(*lv->expression, *rv->expression);
    }
}

void AssertThatDeclarationsMatch(const Declaration& lhs, const Declaration& rhs)
{
    const char* lTy = typeid(lhs).name();
    const char* rTy = typeid(rhs).name();

    if (strcmp(lTy, rTy) != 0)
    {
        cout << "lTy: " << lTy << "\nrTy: " << rTy << "\n";
        ASSERT_EQ(true, false);
    }

    const char* funTy = typeid(FunctionDeclaration).name();
    const char* varTy = typeid(VarDeclaration).name();
    const char* tyTy = typeid(TypeDeclaration).name();

    if (strcmp(lTy, funTy) == 0)
    {
        auto lv = dynamic_cast<const FunctionDeclaration*>(&lhs);
        auto rv = dynamic_cast<const FunctionDeclaration*>(&rhs);
        ASSERT_EQ(lv->decls.size(), rv->decls.size());
        for (unsigned int i = 0; i < lv->decls.size(); ++i)
        {
            const FunDec* ld = &lv->decls[i];
            const FunDec* rd = &rv->decls[i];
            ASSERT_EQ(ld->name, rd->name);
            ASSERT_EQ(ld->resultTy, rd->resultTy);
            AssertThatAstsMatch(*ld->body, *rd->body);
            ASSERT_EQ(ld->fields.size(), rd->fields.size());
            for (unsigned int j = 0; j < ld->fields.size(); ++j)
            {
                ASSERT_EQ(ld->fields[j].name, rd->fields[j].name);
                ASSERT_EQ(ld->fields[j].type, rd->fields[j].type);
            }
        }
    }
    if (strcmp(lTy, varTy) == 0)
    {
        auto lv = dynamic_cast<const VarDeclaration*>(&lhs);
        auto rv = dynamic_cast<const VarDeclaration*>(&rhs);
        ASSERT_EQ(lv->name, rv->name);
        ASSERT_EQ(lv->type, rv->type);
        AssertThatAstsMatch(*lv->init, *rv->init);
    }
    if (strcmp(lTy, tyTy) == 0)
    {
        auto lv = dynamic_cast<const TypeDeclaration*>(&lhs);
        auto rv = dynamic_cast<const TypeDeclaration*>(&rhs);
        ASSERT_EQ(lv->types.size(), rv->types.size());
        for (unsigned int i = 0 ; i < lv->types.size(); ++i)
        {
            ASSERT_EQ(lv->types[i].name, rv->types[i].name);
            AssertThatTypesMatch(*lv->types[i].type, *rv->types[i].type);
        }
    }
}

void AssertThatAstsMatch(const Expression& lhs, const Expression& rhs)
{
    const char* lTy = typeid(lhs).name();
    const char* rTy = typeid(rhs).name();

    if (strcmp(lTy, rTy) != 0)
    {
        cout << "lTy: " << lTy << "\nrTy: " << rTy << "\n";
        ASSERT_EQ(true, false);
    }

    const char* varTy = typeid(VarExpression).name();
    const char* nilTy = typeid(NilExpression).name();
    const char* intTy = typeid(IntExpression).name();
    const char* strTy = typeid(StringExpression).name();
    const char* calTy = typeid(CallExpression).name();
    const char*  opTy = typeid(OpExpression).name();
    const char* recTy = typeid(RecordExpression).name();
    const char* seqTy = typeid(SeqExpression).name();
    const char* assTy = typeid(AssignmentExpression).name();
    const char*  ifTy = typeid(IfExpression).name();
    const char* whlTy = typeid(WhileExpression).name();
    const char* forTy = typeid(ForExpression).name();
    const char* brkTy = typeid(BreakExpression).name();
    const char* letTy = typeid(LetExpression).name();
    const char* arrTy = typeid(ArrayExpression).name();

    if (strcmp(lTy, varTy) == 0)
    {
        return;
    }
    if (strcmp(lTy, nilTy) == 0)
    {
        // always matches
        return;
    }
    if (strcmp(lTy, intTy) == 0)
    {
        auto lxp = dynamic_cast<const IntExpression*>(&lhs);
        auto rxp = dynamic_cast<const IntExpression*>(&rhs);
        ASSERT_EQ(lxp->value, rxp->value);
    }
    if (strcmp(lTy, strTy) == 0)
    {
        auto lxp = dynamic_cast<const StringExpression*>(&lhs);
        auto rxp = dynamic_cast<const StringExpression*>(&rhs);
        ASSERT_EQ(lxp->value, rxp->value);
    }
    if (strcmp(lTy, calTy) == 0)
    {
        auto lxp = dynamic_cast<const CallExpression*>(&lhs);
        auto rxp = dynamic_cast<const CallExpression*>(&rhs);
        ASSERT_EQ(lxp->function, rxp->function);
        ASSERT_EQ(lxp->args.size(), rxp->args.size());
        for (unsigned int i = 0; i < lxp->args.size(); i++)
        {
            AssertThatAstsMatch(*lxp->args[i], *rxp->args[i]);
        }
    }
    if (strcmp(lTy, opTy) == 0)
    {
        auto lxp = dynamic_cast<const OpExpression*>(&lhs);
        auto rxp = dynamic_cast<const OpExpression*>(&rhs);
        ASSERT_EQ(static_cast<int>(lxp->op), static_cast<int>(rxp->op));
        AssertThatAstsMatch(*lxp->lhs, *rxp->lhs);
        AssertThatAstsMatch(*lxp->rhs, *rxp->rhs);
    } 
    if (strcmp(lTy, recTy) == 0)
    {
        auto lxp = dynamic_cast<const RecordExpression*>(&lhs);
        auto rxp = dynamic_cast<const RecordExpression*>(&rhs);
        ASSERT_EQ(lxp->type, rxp->type);
        ASSERT_EQ(lxp->fields.size(), rxp->fields.size());
        for (unsigned int i = 0; i < lxp->fields.size(); ++i)
        {
            ASSERT_EQ(lxp->fields[i].field, rxp->fields[i].field);
            AssertThatAstsMatch(
                    *lxp->fields[i].expr,
                    *rxp->fields[i].expr);
        }
    }
    if (strcmp(lTy, seqTy) == 0)
    {
        auto lxp = dynamic_cast<const SeqExpression*>(&lhs);
        auto rxp = dynamic_cast<const SeqExpression*>(&rhs);
        ASSERT_EQ(lxp->expressions.size(), rxp->expressions.size());
        for (unsigned int i = 0; i < lxp->expressions.size(); ++i)
        {
            AssertThatAstsMatch(
                    *lxp->expressions[i],
                    *rxp->expressions[i]);
        }
    }
    if (strcmp(lTy, assTy) == 0)
    {
        auto lxp = dynamic_cast<const AssignmentExpression*>(&lhs);
        auto rxp = dynamic_cast<const AssignmentExpression*>(&rhs);
        AssertThatVarsMatch(*lxp->var, *rxp->var);
        AssertThatAstsMatch(*lxp->expression, *rxp->expression);
    }
    if (strcmp(lTy, ifTy) == 0)
    {
        auto lxp = dynamic_cast<const IfExpression*>(&lhs);
        auto rxp = dynamic_cast<const IfExpression*>(&rhs);
        AssertThatAstsMatch(*lxp->test, *rxp->test);
        AssertThatAstsMatch(*lxp->thenBranch, *rxp->thenBranch);
        if (!lxp->elseBranch)
        {
            ASSERT_EQ(nullptr, rxp->elseBranch.get());
        }
        else
        {
            AssertThatAstsMatch(*lxp->elseBranch, *rxp->elseBranch);
        }
    }
    if (strcmp(lTy, whlTy) == 0)
    {
        auto lxp = dynamic_cast<const WhileExpression*>(&lhs);
        auto rxp = dynamic_cast<const WhileExpression*>(&rhs);
        AssertThatAstsMatch(*lxp->test, *rxp->test);
        AssertThatAstsMatch(*lxp->body, *rxp->body);
    }
    if (strcmp(lTy, forTy) == 0)
    {
        auto lxp = dynamic_cast<const ForExpression*>(&lhs);
        auto rxp = dynamic_cast<const ForExpression*>(&rhs);
        ASSERT_EQ(lxp->var, rxp->var);
        AssertThatAstsMatch(*lxp->high, *rxp->high);
        AssertThatAstsMatch(*lxp->low, *rxp->low);
        AssertThatAstsMatch(*lxp->body, *rxp->body);
    }
    if (strcmp(lTy, brkTy) == 0)
    {
       return; 
    }
    if (strcmp(lTy, letTy) == 0)
    {
        auto lxp = dynamic_cast<const LetExpression*>(&lhs);
        auto rxp = dynamic_cast<const LetExpression*>(&rhs);
        AssertThatAstsMatch(*lxp->body, *rxp->body);
        ASSERT_EQ(lxp->decls.size(), rxp->decls.size());
        for (unsigned int i = 0 ; i < lxp->decls.size(); ++i)
        {
            AssertThatDeclarationsMatch(*lxp->decls[i], *rxp->decls[i]);
        }
    }
    if (strcmp(lTy, arrTy) == 0)
    {
        auto lxp = dynamic_cast<const ArrayExpression*>(&lhs);
        auto rxp = dynamic_cast<const ArrayExpression*>(&rhs);
        ASSERT_EQ(lxp->type, rxp->type);
        AssertThatAstsMatch(*lxp->size, *rxp->size);
        AssertThatAstsMatch(*lxp->init, *rxp->init);
    }
}


class ParserTest : public ::testing::Test
{
public:
    void SetUp() override { parser = nullptr; }
    void CreateFromString(const string& program);
    std::unique_ptr<Parser> parser;
};

void ParserTest::CreateFromString(const string& program)
{
    unique_ptr<istream> stream = make_unique<stringstream>(program);
    TokenStream tokenStream(move(stream));
    parser = make_unique<Parser>(move(tokenStream), make_shared<CompileTimeErrorReporter>(), make_shared<WarningReporter>());
}

TEST_F(ParserTest, NilExp_noParens) 
{
    CreateFromString("nil");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> nil = MakeTestProgram(make_unique<NilExpression>(Position{0,0}));
    AssertThatAstsMatch(nil->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, IntExp_noParens)
{
    CreateFromString("4");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> four = MakeTestProgram(make_unique<IntExpression>(4, Position(0,0)));
    AssertThatAstsMatch(prog->UseExpression(), four->UseExpression());
}

TEST_F(ParserTest, SimpleArithExp_noParens)
{
    CreateFromString("4 + 5");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<OpExpression>(
        make_unique<IntExpression>(4, Position(0,0)),
        make_unique<IntExpression>(5, Position(0,0)),
        BinOp::Plus, Position{0,0}
    ));
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, StringExp)
{
    CreateFromString("\"Hello\"");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<StringExpression>("Hello", Position(0,0)));
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, BreakExp)
{
    CreateFromString("break");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<BreakExpression>(Position{0,0}));
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, UnMatchedParens_throws)
{
    CreateFromString("( int ");
    ASSERT_THROW(parser->Parse(), ParseException);
}

TEST_F(ParserTest, MatchedParens_Int)
{
    CreateFromString("(4)");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> four = MakeTestProgram(make_unique<IntExpression>(4, Position(0,0)));
    AssertThatAstsMatch(prog->UseExpression(), four->UseExpression());
}

TEST_F(ParserTest, NegExpression)
{
    CreateFromString("-6");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<OpExpression>(
        make_unique<IntExpression>(0, Position(0,0)),
        make_unique<IntExpression>(6, Position(0,0)),
        BinOp::Minus, Position{0,0}
    ));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, Unit)
{
    CreateFromString("()");
    unique_ptr<Program> prog = parser->Parse();

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), Position{0,0})
    );
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, SeqUnit)
{
    CreateFromString("(4 ; ())");
    unique_ptr<Program> prog = parser->Parse();

    vector<unique_ptr<Expression>> vec;
    vec.push_back(make_unique<IntExpression>(4, Position(0,0)));
    vec.push_back(make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), Position{0,0}));

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<SeqExpression>(move(vec), Position{0,0})
    );
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, SeqInts)
{
    CreateFromString("(4 ; 5)");
    unique_ptr<Program> prog = parser->Parse();

    vector<unique_ptr<Expression>> vec;
    vec.push_back(make_unique<IntExpression>(4, Position(0,0)));
    vec.push_back(make_unique<IntExpression>(5, Position(0,0)));

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<SeqExpression>(move(vec), Position{0,0})
    );
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, AndWeirdness)
{
    CreateFromString("4 & 7");
    unique_ptr<Program> prog = parser->Parse();

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<IfExpression>(
            make_unique<IntExpression>(4, Position(0,0)),
            make_unique<IntExpression>(7, Position(0,0)),
            make_unique<IntExpression>(0, Position(0,0)), Position{0,0})
        );
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, OrWeirdness)
{
    CreateFromString("4 | 7");
    unique_ptr<Program> prog = parser->Parse();

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<IfExpression>(
            make_unique<IntExpression>(4, Position(0,0)),
            make_unique<IntExpression>(1, Position(0,0)),
            make_unique<IntExpression>(7, Position(0,0)), Position{0,0})
        );
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, IfWithElse)
{
    CreateFromString("if 4 then 7 else 5");
    unique_ptr<Program> prog = parser->Parse();

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<IfExpression>(
            make_unique<IntExpression>(4, Position(0,0)),
            make_unique<IntExpression>(7, Position(0,0)),
            make_unique<IntExpression>(5, Position(0,0)), Position{0,0})
        );
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, IfWithNoElse)
{
    CreateFromString("if 4 then 7");
    unique_ptr<Program> prog = parser->Parse();

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<IfExpression>(
            make_unique<IntExpression>(4, Position(0,0)),
            make_unique<IntExpression>(7, Position(0,0)),
            nullptr, Position{0,0})
        );
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, ArithExp_Precedence)
{
    CreateFromString("4 + 5 * 3");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<OpExpression>(
        make_unique<IntExpression>(4, Position(0,0)),
        make_unique<OpExpression>(
            make_unique<IntExpression>(5, Position(0,0)),
            make_unique<IntExpression>(3, Position(0,0)),
            BinOp::Times, Position{0,0}
        ),
        BinOp::Plus, Position{0,0}
    ));
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, ArithExp_PrecedenceBustingParens)
{
    CreateFromString("(4 + 5) * 3");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<OpExpression>(
        make_unique<OpExpression>(
            make_unique<IntExpression>(4, Position(0,0)),
            make_unique<IntExpression>(5, Position(0,0)),
            BinOp::Plus, Position{0,0}
        ),
        make_unique<IntExpression>(3, Position(0,0)),
        BinOp::Times, Position{0,0}
    ));
    AssertThatAstsMatch(prog->UseExpression(), target->UseExpression());
}

TEST_F(ParserTest, WhileExpr)
{
    CreateFromString("while 1 do 2");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<WhileExpression>(
        make_unique<IntExpression>(1, Position(0,0)),
        make_unique<IntExpression>(2, Position(0,0)), Position{0,0}
        ));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, ForExpr)
{
    CreateFromString("for a := 1 to 2 do 3");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<ForExpression>(
        SymbolFactory::GenerateSymbol("a"),
        make_unique<IntExpression>(1, Position(0,0)),
        make_unique<IntExpression>(2, Position(0,0)),
        make_unique<IntExpression>(3, Position(0,0)), Position{0,0}
        ));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, CallExprNoArgs)
{
    CreateFromString("a()");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(make_unique<CallExpression>(
        SymbolFactory::GenerateSymbol("a"),
        vector<unique_ptr<Expression>>(),
        Position(0,0)));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, CallExprOneArg)
{
    CreateFromString("a(1)");
    unique_ptr<Program> prog = parser->Parse();
    vector<unique_ptr<Expression>> args;
    args.push_back(make_unique<IntExpression>(1, Position(0,0)));
    unique_ptr<Program> target = MakeTestProgram(make_unique<CallExpression>(
        SymbolFactory::GenerateSymbol("a"),
        move(args), Position(0,0)
        ));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}


TEST_F(ParserTest, CallExprManyArgs)
{
    CreateFromString("a(1, 2, 3)");
    unique_ptr<Program> prog = parser->Parse();
    vector<unique_ptr<Expression>> args;
    args.push_back(make_unique<IntExpression>(1, Position(0,0)));
    args.push_back(make_unique<IntExpression>(2, Position(0,0)));
    args.push_back(make_unique<IntExpression>(3, Position(0,0)));
    unique_ptr<Program> target = MakeTestProgram(make_unique<CallExpression>(
        SymbolFactory::GenerateSymbol("a"),
        move(args), Position(0,0)
        ));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, BigSeq)
{
    CreateFromString("(a := 5; a + 1)");
    unique_ptr<Program> prog = parser->Parse();

    vector<unique_ptr<Expression>> exprs;
    exprs.push_back(make_unique<AssignmentExpression>(
        make_unique<SimpleVar>(SymbolFactory::GenerateSymbol("a"), Position{0,0}),
        make_unique<IntExpression>(5, Position(0,0)), Position{0,0}));

    exprs.push_back(make_unique<OpExpression>(
        make_unique<VarExpression>(make_unique<SimpleVar>(SymbolFactory::GenerateSymbol("a"), Position{0,0}), Position{0,0}),
        make_unique<IntExpression>(1, Position(0,0)),
        BinOp::Plus, Position{0,0}
        ));

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<SeqExpression>(move(exprs), Position{0,0}));

    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, LetFunDecs)
{
    CreateFromString("let var a := 5 function f() : int = g(a) function g(i : int) = f() in f() end");
    unique_ptr<Program> prog = parser->Parse();

    vector<FunDec> fundecs;

    vector<unique_ptr<Expression>> args;
    args.push_back(make_unique<VarExpression>(make_unique<SimpleVar>(SymbolFactory::GenerateSymbol("a"), Position{0,0}), Position{0,0}));

    fundecs.push_back(FunDec(SymbolFactory::GenerateSymbol("f"), vector<Field>(), SymbolFactory::GenerateSymbol("int"), make_unique<CallExpression>(SymbolFactory::GenerateSymbol("g"), move(args), Position(0,0)), Position(0,0)));

    fundecs.push_back(FunDec(SymbolFactory::GenerateSymbol("g"), { Field(SymbolFactory::GenerateSymbol("i"), SymbolFactory::GenerateSymbol("int"), Position{0,0}) }, boost::optional<Symbol>(), make_unique<CallExpression>(SymbolFactory::GenerateSymbol("f"), vector<unique_ptr<Expression>>(), Position(0,0)), Position{0,0}));

    vector<unique_ptr<Declaration>> decs;
    decs.push_back(make_unique<VarDeclaration>(SymbolFactory::GenerateSymbol("a"), boost::optional<Symbol>(), make_unique<IntExpression>(5, Position(0,0)), Position{0,0}));
    decs.push_back(make_unique<FunctionDeclaration>(move(fundecs), Position{0,0}));

    unique_ptr<Program> target = MakeTestProgram(make_unique<LetExpression>(
        move(decs),
        make_unique<CallExpression>(SymbolFactory::GenerateSymbol("f"), vector<unique_ptr<Expression>>(), Position(0,0)), Position{0,0}));

    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, EmptyLet)
{
    CreateFromString("let in end");
    unique_ptr<Program> prog = parser->Parse();
    unique_ptr<Program> target = MakeTestProgram(
        make_unique<LetExpression>(vector<unique_ptr<Declaration>>(), make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), Position{0,0}), Position{0,0}));
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}

TEST_F(ParserTest, TypeDecs)
{
    CreateFromString("let type tree = {key: int, children: treelist} type treelist = {head: tree, tail: treelist} in end");
    unique_ptr<Program> prog = parser->Parse();

    vector<TyDec> tyDecs;
    tyDecs.push_back(TyDec(SymbolFactory::GenerateSymbol("tree"), make_unique<RecordType>( vector<Field>{ Field(SymbolFactory::GenerateSymbol("key"), SymbolFactory::GenerateSymbol("int"), Position{0,0}) , Field(SymbolFactory::GenerateSymbol("children"), SymbolFactory::GenerateSymbol("treelist"), Position{0,0}) }, Position{0,0}), Position{0,0}));
    tyDecs.push_back(TyDec(SymbolFactory::GenerateSymbol("treelist"), make_unique<RecordType>( vector<Field>{ Field(SymbolFactory::GenerateSymbol("head"), SymbolFactory::GenerateSymbol("tree"), Position{0,0}), Field(SymbolFactory::GenerateSymbol("tail"), SymbolFactory::GenerateSymbol("treelist"), Position{0,0}) } , Position{0,0}), Position{0,0}));

    vector<unique_ptr<Declaration>> decls;
    decls.push_back(make_unique<TypeDeclaration>(move(tyDecs), Position{0,0}));

    unique_ptr<Program> target = MakeTestProgram(
        make_unique<LetExpression>(move(decls), make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), Position(0,0)), Position{0,0}));
    
    AssertThatAstsMatch(target->UseExpression(), prog->UseExpression());
}
