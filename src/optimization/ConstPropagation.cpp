#include "ConstPropagation.hpp"
#include "logging.hpp"
using namespace std;
#define i2f(ins,m_) (ConstantFP::get((float)cast_constantint(ins->get_operand(0))->get_value(), m_))
#define f2i(ins,m_) (ConstantInt::get((int)cast_constantfp(ins->get_operand(0))->get_value(), m_))
#define zext(ins,m_) (ConstantInt::get((int)cast_constantint(ins->get_operand(0))->get_value(), m_))
#define isint(operand) (cast_constantint(operand))
#define isfloat(operand) (cast_constantfp(operand))

// 给出了返回整形值的常数折叠实现，大家可以参考，在此基础上拓展
// 当然如果同学们有更好的方式，不强求使用下面这种方式
ConstantInt *ConstFolder::compute(
    Instruction::OpID op,
    ConstantInt *value1,
    ConstantInt *value2)
{

    int c_value1 = value1->get_value();
    int c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::add:
        return ConstantInt::get(c_value1 + c_value2, module_);

        break;
    case Instruction::sub:
        return ConstantInt::get(c_value1 - c_value2, module_);
        break;
    case Instruction::mul:
        return ConstantInt::get(c_value1 * c_value2, module_);
        break;
    case Instruction::sdiv:
        return ConstantInt::get((int)(c_value1 / c_value2), module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantFP *ConstFolder::compute_float(
    Instruction::OpID op,
    ConstantFP *value1,
    ConstantFP *value2)
{

    float c_value1 = value1->get_value();
    float c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::fadd:
        return ConstantFP::get((float)(c_value1 + c_value2), module_);

        break;
    case Instruction::fsub:
        return ConstantFP::get((float)(c_value1 - c_value2), module_);
        break;
    case Instruction::fmul:
        return ConstantFP::get((float)(c_value1 * c_value2), module_);
        break;
    case Instruction::fdiv:
        return ConstantFP::get((float)(c_value1 / c_value2), module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantInt *ConstFolder::intcmp(
    CmpInst::CmpOp op, 
    ConstantInt *value1,
    ConstantInt *value2) 
{
    int c_value1 = value1->get_value();
    int c_value2 = value2->get_value();
    switch (op) {
        case CmpInst::EQ:
            return ConstantInt::get((int)(c_value1 == c_value2), module_);
            break;
        case CmpInst::NE:
            return ConstantInt::get((int)(c_value1 != c_value2), module_);
            break;
        case CmpInst::LE:
            return ConstantInt::get((int)(c_value1 <= c_value2), module_);
            break;
        case CmpInst::LT:
            return ConstantInt::get((int)(c_value1 < c_value2), module_);
            break;
        case CmpInst::GT:
            return ConstantInt::get((int)(c_value1 > c_value2), module_);
            break;
        case CmpInst::GE:
            return ConstantInt::get((int)(c_value1 >= c_value2), module_);
            break;
        default:
            return nullptr;
            break;
    }
}

ConstantInt *ConstFolder::floatcmp(
    FCmpInst::CmpOp op, 
    ConstantFP *value1,
    ConstantFP *value2) 
{
    float c_value1 = value1->get_value();
    float c_value2 = value2->get_value();
    switch (op) {
        case CmpInst::EQ:
            return ConstantInt::get((int)(c_value1 == c_value2), module_);
            break;
        case CmpInst::NE:
            return ConstantInt::get((int)(c_value1 != c_value2), module_);
            break;
        case CmpInst::LE:
            return ConstantInt::get((int)(c_value1 <= c_value2), module_);
            break;
        case CmpInst::LT:
            return ConstantInt::get((int)(c_value1 < c_value2), module_);
            break;
        case CmpInst::GT:
            return ConstantInt::get((int)(c_value1 > c_value2), module_);
            break;
        case CmpInst::GE:
            return ConstantInt::get((int)(c_value1 >= c_value2), module_);
            break;
        default:
            return nullptr;
            break;
    }
}

// 用来判断value是否为ConstantFP，如果不是则会返回nullptr
ConstantFP *cast_constantfp(Value *value)
{
    auto constant_fp_ptr = dynamic_cast<ConstantFP *>(value);
    if (constant_fp_ptr)
    {
        return constant_fp_ptr;
    }
    else
    {
        return nullptr;
    }
}
ConstantInt *cast_constantint(Value *value)
{
    auto constant_int_ptr = dynamic_cast<ConstantInt *>(value);
    if (constant_int_ptr)
    {
        return constant_int_ptr;
    }
    else
    {
        return nullptr;
    }
}


void ConstPropagation::run()
{
    // 从这里开始吧！
    ConstFolder *fold = new ConstFolder(m_);
    auto func_lists=m_->get_functions();
    for(auto func: func_lists){
        if(func->get_basic_blocks().size()==0)
        {
            continue;
        }
        else
        {
            for (auto bb:func->get_basic_blocks()){
                std::vector<Instruction*> del;
                for(Instruction* ins: bb->get_instructions()){
                    if(ins->isBinary()){
                        auto left=ins->get_operand(0);
                        auto right=ins->get_operand(1);
                        if(isint(left)!=nullptr&&isint(right)!=nullptr){
                            auto intfold=fold->compute(ins->get_instr_type(),isint(left),isint(right));
                            ins->replace_all_use_with(intfold);
                            del.push_back(ins);
                        }
                        else if(isfloat(left)!=nullptr&&isfloat(right)!=nullptr){
                            auto floatfold=fold->compute_float(ins->get_instr_type(),isfloat(left),isfloat(right));
                            ins->replace_all_use_with(floatfold);
                            del.push_back(ins);
                        }
                        else{
                            if(isfloat(left)!=nullptr&&isint(right)!=nullptr){
                                float temp=(float)isint(right)->get_value();
                                auto op=ConstantFP::get(temp,m_);
                                auto floatfold=fold->compute_float(ins->get_instr_type(),isfloat(left),op);
                                ins->replace_all_use_with(floatfold);
                                del.push_back(ins);
                            }
                            else if(isint(left)!=nullptr&&isfloat(right)!=nullptr){
                                float temp=(float)isint(left)->get_value();
                                auto op=ConstantFP::get(temp,m_);
                                auto floatfold=fold->compute_float(ins->get_instr_type(),op,isfloat(right));
                                ins->replace_all_use_with(floatfold);
                                del.push_back(ins);
                            }
                        }
                    }
                    else if(ins->is_cmp()){
                        auto left=ins->get_operand(0);
                        auto right=ins->get_operand(1);
                        if(isint(left)!=nullptr&&isint(right)!=nullptr){
                            auto intfold=fold->intcmp(dynamic_cast<CmpInst*>(ins)->get_cmp_op(),isint(left),isint(right));
                            ins->replace_all_use_with(intfold);
                            del.push_back(ins);
                        }
                        else{
                            if(isfloat(left)!=nullptr&&isint(right)!=nullptr){
                                int temp=(int)isint(left)->get_value();
                                auto op=ConstantInt::get(temp,m_);
                                auto intfold=fold->intcmp(dynamic_cast<CmpInst*>(ins)->get_cmp_op(),isint(left),op);
                                ins->replace_all_use_with(intfold);
                                del.push_back(ins);
                            }
                            else if(isint(left)!=nullptr&&isfloat(right)!=nullptr){
                                int temp=(int)isint(right)->get_value();
                                auto op=ConstantInt::get(temp,m_);
                                auto intfold=fold->intcmp(dynamic_cast<CmpInst*>(ins)->get_cmp_op(),op,isint(right));
                                ins->replace_all_use_with(intfold);
                                del.push_back(ins);
                            }
                        }
                    }
                    else if(ins->is_fcmp()){
                        auto left=ins->get_operand(0);
                        auto right=ins->get_operand(1);
                        if(isfloat(left)!=nullptr&&isfloat(right)!=nullptr){
                            auto floatfold=fold->floatcmp(dynamic_cast<FCmpInst*>(ins)->get_cmp_op(),isfloat(left),isfloat(right));
                            ins->replace_all_use_with(floatfold);
                            del.push_back(ins);
                        }
                        else{
                            if(isfloat(left)!=nullptr&&isint(right)!=nullptr){
                                float temp=(float)isint(right)->get_value();
                                auto op=ConstantFP::get(temp,m_);
                                auto floatfold=fold->floatcmp(dynamic_cast<FCmpInst*>(ins)->get_cmp_op(),isfloat(left),op);
                                ins->replace_all_use_with(floatfold);
                                del.push_back(ins);
                            }
                            else if(isint(left)!=nullptr&&isfloat(right)!=nullptr){
                                float temp=(float)isint(left)->get_value();
                                auto op=ConstantFP::get(temp,m_);
                                auto floatfold=fold->floatcmp(dynamic_cast<FCmpInst*>(ins)->get_cmp_op(),op,isfloat(right));
                                ins->replace_all_use_with(floatfold);
                                del.push_back(ins);
                            }
                        }
                    }
                    else if(ins->is_zext()){
                        if(isint(ins->get_operand(0))!=nullptr){
                            ConstantInt *val_get=zext(ins,m_);
                            ins->replace_all_use_with(val_get);
                            del.push_back(ins);
                        }
                    }
                    else if(ins->is_si2fp()){
                        if(isint(ins->get_operand(0))!=nullptr){
                            ConstantFP *val_get=i2f(ins,m_);
                            ins->replace_all_use_with(val_get);
                            del.push_back(ins);
                        }
                    }
                    else if(ins->is_fp2si()){;
                        if(isfloat(ins->get_operand(0))!=nullptr){
                            ConstantInt *val_get=f2i(ins,m_);
                            ins->replace_all_use_with(val_get);
                            del.push_back(ins);
                        }
                    } 
                }
                for(Instruction* ins: del){
                    bb->delete_instr(ins);
                }
            }
        }
    }
}
