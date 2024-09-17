
#include <clang/AST/AST.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/DeclarationName.h>
#include <clang/AST/Expr.h>
#include <clang/Basic/IdentifierTable.h>
#include <clang/Basic/PartialDiagnostic.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/Specifiers.h>
#include <clang/Sema/DeclSpec.h>
#include <clang/Serialization/ASTReader.h>
#include <cstdint>
#include <llvm/ADT/APInt.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/Decl.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/ParsedAttrInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;


//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class ReflectorASTConsumer: public clang::ASTConsumer {
public:
  explicit ReflectorASTConsumer(CompilerInstance* CI, ASTContext* Ctx)
      : Compiler(CI),
        Context(Ctx) {

    if (Context->getCurrentNamedModule() && checkModuleUsable(Context->getCurrentNamedModule())) {
      llvm::outs() << "[WARN] MODULE USABLE " << "\n";
    }
  }

  void add_types_decl(CXXRecordDecl* record, const std::list<QualType>& field_types) {
    // Create the using field_types = pack<int, long, double> statement
    IdentifierInfo&          FieldTypesID = Context->Idents.get("field_types");
    IdentifierInfo&          PackID       = Context->Idents.get("refl_pack");

    // Create Template Specialization for 'pack<int, long, double>'
    TemplateName             PackTemplate{dyn_cast<TemplateDecl>(
      Compiler->getSema().getAsTemplateNameDecl(Compiler->getSema().LookupSingleName(
        Compiler->getSema().getCurScope(), {&PackID}, SourceLocation(), Sema::LookupOrdinaryName
      )))
    };
    TemplateArgumentListInfo TemplateArgs{SourceLocation(), SourceLocation()};
    for (const auto& type: field_types) {
      TemplateArgs.addArgument(
        TemplateArgumentLoc(TemplateArgument(type), Context->getTrivialTypeSourceInfo(type))
      );
    }

    QualType PackSpecialization =
      Compiler->getSema().CheckTemplateIdType(PackTemplate, SourceLocation(), TemplateArgs);

    // Create TypeAliasDecl for 'using field_types = pack<int, long, double>'
    TypeAliasDecl* FieldTypesAlias = TypeAliasDecl::Create(
      *Context,
      record,
      SourceLocation(),
      SourceLocation(),
      &FieldTypesID,
      Context->getTrivialTypeSourceInfo(PackSpecialization)
    );

    FieldTypesAlias->setAccess(AccessSpecifier::AS_public);

    // Add field_types to the struct
    record->addDecl(FieldTypesAlias);
  }

  void add_integer_list(CXXRecordDecl* record, const std::list<uint64_t>& items, const std::string& identifier) {
    // Create the using field_types = pack<int, long, double> statement
    IdentifierInfo&          FieldTypesID = Context->Idents.get(identifier);
    IdentifierInfo&          PackID       = Context->Idents.get("refl_int_pack");

    QualType                 type         = Context->UnsignedLongTy;

    // Create Template Specialization for 'pack<int, long, double>'
    TemplateName             PackTemplate{dyn_cast<TemplateDecl>(
      Compiler->getSema().getAsTemplateNameDecl(Compiler->getSema().LookupSingleName(
        Compiler->getSema().getCurScope(), {&PackID}, SourceLocation(), Sema::LookupOrdinaryName
      ))
    )};
    TemplateArgumentListInfo TemplateArgs{SourceLocation(), SourceLocation()};
    for (const auto& item: items) {
      auto value = llvm::APSInt(std::to_string(item));
      auto* item_expr = IntegerLiteral::Create(*Context, value, type, SourceLocation());
      TemplateArgs.addArgument(TemplateArgumentLoc(
        TemplateArgument(*Context, value, type),
        item_expr
      ));
    }

    QualType PackSpecialization =
      Compiler->getSema().CheckTemplateIdType(PackTemplate, SourceLocation(), TemplateArgs);

    // Create TypeAliasDecl for 'using field_types = pack<int, long, double>'
    TypeAliasDecl* FieldTypesAlias = TypeAliasDecl::Create(
      *Context,
      record,
      SourceLocation(),
      SourceLocation(),
      &FieldTypesID,
      Context->getTrivialTypeSourceInfo(PackSpecialization)
    );

    FieldTypesAlias->setAccess(AccessSpecifier::AS_public);

    // Add field_types to the struct
    record->addDecl(FieldTypesAlias);
  }

  void add_names_decl(CXXRecordDecl* record, const std::list<std::string>& names) {
    IdentifierInfo& FieldNamesID          = Context->Idents.get("field_names");
    QualType        ConstCharPtrArrayType = Context->getConstantArrayType(
      Context->getPointerType(Context->CharTy.withConst()),
       llvm::APInt(32, names.size()),
       nullptr,
        ArraySizeModifier::Normal,
         0
    );
    TypeSourceInfo* TSI           = Context->getTrivialTypeSourceInfo(ConstCharPtrArrayType);

    VarDecl* FieldNamesVar = VarDecl::Create(
      *Context,
      record,
      SourceLocation(),
      SourceLocation(),
      &FieldNamesID,
      ConstCharPtrArrayType,
      nullptr,
      SC_Static
    );
    FieldNamesVar->setInitStyle(VarDecl::CInit);
    FieldNamesVar->setTypeSourceInfo(TSI);

    // Create the array initializer
    SmallVector<Expr*, 1> init_exprs{};
    for (const auto& name: names) {
      init_exprs.push_back(StringLiteral::Create(
        *Context,
        name,
        StringLiteralKind::Ordinary,
        false,
        Context->getStringLiteralArrayType(Context->CharTy.withConst(), name.size()),
        SourceLocation()
      ));
    }


    auto* FieldNamesInitList = new (Context) InitListExpr(*Context, SourceLocation(), init_exprs, SourceLocation());

    FieldNamesInitList->setType(ConstCharPtrArrayType);
    FieldNamesVar->setConstexpr(true);
    FieldNamesVar->setAccess(AccessSpecifier::AS_public);
    Compiler->getSema().AddInitializerToDecl(FieldNamesVar, FieldNamesInitList, false);

    // // Add field_names to the struct
    record->addDecl(FieldNamesVar);
  }

  void add_type_info(CXXRecordDecl* record) {
    std::list<std::string> field_names{};
    std::list<uint64_t>    field_sizes{};
    std::list<uint64_t>    field_offsets{};
    std::list<uint64_t>    field_accesses{};
    std::list<QualType>    field_types{};

    for (const auto& field: record->fields()) {
      if (field->isTemplated() || field->isTemplateDecl()) continue;

      auto name   = field->getNameAsString();
      auto type   = field->getType();
      auto size   = Context->getTypeSize(type);
      auto offset = Context->getFieldOffset(field);
      offset /= 8;

      uint64_t access = 0;
      switch (field->getAccess()) {
        case AS_public:
          access = 3;
          break;
        case AS_protected:
          access = 2;
          break;
        case AS_private:
          access = 1;
          break;
        case AS_none:
          access = 0;
          break;
      }

      field_names.push_back(name);
      field_sizes.push_back(size);
      field_offsets.push_back(offset);
      field_types.push_back(type);
      field_accesses.push_back(access);
    }

    if (!field_names.empty()) {
      IdentifierInfo& type_info_id     = Context->Idents.get("__type_info__");
      CXXRecordDecl*  type_info_record = CXXRecordDecl::Create(
        *Context,
        CXXRecordDecl::TagKind::Class,
        record,
        record->getEndLoc(),
        record->getEndLoc(),
        &type_info_id
      );

      type_info_record->startDefinition();

      add_names_decl(type_info_record, field_names);
      add_types_decl(type_info_record, field_types);
      add_integer_list(type_info_record, field_sizes, "field_sizes");
      add_integer_list(type_info_record, field_offsets, "field_offsets");
      add_integer_list(type_info_record, field_accesses, "field_access_specifiers");

      type_info_record->setAccess(AccessSpecifier::AS_public);
      type_info_record->completeDefinition();
      Compiler->getSema().CheckCompletedCXXClass(
        Compiler->getSema().getCurScope(), type_info_record
      );
      record->addDecl(type_info_record);
    }
  }

  void HandleTagDeclDefinition(TagDecl* D) override {
    if (!module_usable &&
        ((D->getOwningModule() == nullptr) ||
         D->getOwningModule()->getPrimaryModuleInterfaceName() != "reflect")) {
      return;
    }
    
    if ((D->isStruct() || D->isClass()) && (D->getDefinition() == D) && !D->isEnum()
    /* &&
        !D->isInAnotherModuleUnit() &&
        (D->hasOwningModule() && !D->getOwningModule()->isGlobalModule())*/) {
      auto*        record = dyn_cast<CXXRecordDecl>(D);
      add_type_info(record);
    }
  }

  bool HandleTopLevelDecl(DeclGroupRef D) override {
    // llvm::outs() << "HANDLING TOP LEVEL DECLARATION" << "\n";
    // IdentifierInfo& PackID = Context->Idents.get("pack");
    // auto*           lookup = Compiler->getSema().LookupSingleName(
    //   Compiler->getSema().getCurScope(), {&PackID}, SourceLocation(), Sema::LookupOrdinaryName
    // );
    // if (lookup != nullptr) {
    //   llvm::outs() << "[WARN] MODULE USABLE " << "\n";
    //   module_usable = true;
    //   return true;
    // }

    // return true;
    for (const auto& d: D) {
      if (d->getKind() == Decl::Import) {
        ImportDecl* id = dyn_cast<ImportDecl>(d);
        // llvm::outs() << "[ERROR] import "
        //              << id->getImportedModule()->getPrimaryModuleInterfaceName() << "\n";
        if (!module_usable && checkModuleUsable(id->getImportedModule())) {
          // llvm::outs() << "[WARN] MODULE USABLE " << "\n";
        }
      }

      // d->print(llvm::outs());
    }
    return true;
  }

private:
  bool checkModuleUsable(Module* m, unsigned int indent = 0) {
    // for (unsigned int i = 0; i < indent; ++i) {
    //   llvm::outs() << "  ";
    // }
    // llvm::outs() << m->getFullModuleName() << "\n";

    if (m->getPrimaryModuleInterfaceName() == "reflect") {
      refl_module   = m;
      module_usable = true;
      return true;
    }

    SmallVector<Module*> exported_modules{};
    m->getExportedModules(exported_modules);
    for (const auto& exported_module: exported_modules) {
      if (checkModuleUsable(exported_module, indent + 1)) {
        return true;
      }
    }

    return false;
  }

private:
  CompilerInstance* Compiler;
  ASTContext*       Context;
  bool              module_usable = false;
  Module*           refl_module   = nullptr;
};
