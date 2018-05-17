// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME bindIdict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "src/classes.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *Object_Dictionary();
   static void Object_TClassManip(TClass*);
   static void *new_Object(void *p = 0);
   static void *newArray_Object(Long_t size, void *p);
   static void delete_Object(void *p);
   static void deleteArray_Object(void *p);
   static void destruct_Object(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Object*)
   {
      ::Object *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Object));
      static ::ROOT::TGenericClassInfo 
         instance("Object", "Object.hpp", 10,
                  typeid(::Object), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Object_Dictionary, isa_proxy, 4,
                  sizeof(::Object) );
      instance.SetNew(&new_Object);
      instance.SetNewArray(&newArray_Object);
      instance.SetDelete(&delete_Object);
      instance.SetDeleteArray(&deleteArray_Object);
      instance.SetDestructor(&destruct_Object);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Object*)
   {
      return GenerateInitInstanceLocal((::Object*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Object*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Object_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Object*)0x0)->GetClass();
      Object_TClassManip(theClass);
   return theClass;
   }

   static void Object_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Jet_Dictionary();
   static void Jet_TClassManip(TClass*);
   static void *new_Jet(void *p = 0);
   static void *newArray_Jet(Long_t size, void *p);
   static void delete_Jet(void *p);
   static void deleteArray_Jet(void *p);
   static void destruct_Jet(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Jet*)
   {
      ::Jet *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Jet));
      static ::ROOT::TGenericClassInfo 
         instance("Jet", "Jet.hpp", 15,
                  typeid(::Jet), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Jet_Dictionary, isa_proxy, 4,
                  sizeof(::Jet) );
      instance.SetNew(&new_Jet);
      instance.SetNewArray(&newArray_Jet);
      instance.SetDelete(&delete_Jet);
      instance.SetDeleteArray(&deleteArray_Jet);
      instance.SetDestructor(&destruct_Jet);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Jet*)
   {
      return GenerateInitInstanceLocal((::Jet*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Jet*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Jet_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Jet*)0x0)->GetClass();
      Jet_TClassManip(theClass);
   return theClass;
   }

   static void Jet_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *FatJet_Dictionary();
   static void FatJet_TClassManip(TClass*);
   static void *new_FatJet(void *p = 0);
   static void *newArray_FatJet(Long_t size, void *p);
   static void delete_FatJet(void *p);
   static void deleteArray_FatJet(void *p);
   static void destruct_FatJet(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::FatJet*)
   {
      ::FatJet *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::FatJet));
      static ::ROOT::TGenericClassInfo 
         instance("FatJet", "FatJet.hpp", 14,
                  typeid(::FatJet), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &FatJet_Dictionary, isa_proxy, 4,
                  sizeof(::FatJet) );
      instance.SetNew(&new_FatJet);
      instance.SetNewArray(&newArray_FatJet);
      instance.SetDelete(&delete_FatJet);
      instance.SetDeleteArray(&deleteArray_FatJet);
      instance.SetDestructor(&destruct_FatJet);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::FatJet*)
   {
      return GenerateInitInstanceLocal((::FatJet*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::FatJet*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *FatJet_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::FatJet*)0x0)->GetClass();
      FatJet_TClassManip(theClass);
   return theClass;
   }

   static void FatJet_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Lepton_Dictionary();
   static void Lepton_TClassManip(TClass*);
   static void *new_Lepton(void *p = 0);
   static void *newArray_Lepton(Long_t size, void *p);
   static void delete_Lepton(void *p);
   static void deleteArray_Lepton(void *p);
   static void destruct_Lepton(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Lepton*)
   {
      ::Lepton *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Lepton));
      static ::ROOT::TGenericClassInfo 
         instance("Lepton", "Lepton.hpp", 8,
                  typeid(::Lepton), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Lepton_Dictionary, isa_proxy, 4,
                  sizeof(::Lepton) );
      instance.SetNew(&new_Lepton);
      instance.SetNewArray(&newArray_Lepton);
      instance.SetDelete(&delete_Lepton);
      instance.SetDeleteArray(&deleteArray_Lepton);
      instance.SetDestructor(&destruct_Lepton);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Lepton*)
   {
      return GenerateInitInstanceLocal((::Lepton*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Lepton*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Lepton_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Lepton*)0x0)->GetClass();
      Lepton_TClassManip(theClass);
   return theClass;
   }

   static void Lepton_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *GenParticle_Dictionary();
   static void GenParticle_TClassManip(TClass*);
   static void *new_GenParticle(void *p = 0);
   static void *newArray_GenParticle(Long_t size, void *p);
   static void delete_GenParticle(void *p);
   static void deleteArray_GenParticle(void *p);
   static void destruct_GenParticle(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GenParticle*)
   {
      ::GenParticle *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::GenParticle));
      static ::ROOT::TGenericClassInfo 
         instance("GenParticle", "GenParticle.hpp", 7,
                  typeid(::GenParticle), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &GenParticle_Dictionary, isa_proxy, 4,
                  sizeof(::GenParticle) );
      instance.SetNew(&new_GenParticle);
      instance.SetNewArray(&newArray_GenParticle);
      instance.SetDelete(&delete_GenParticle);
      instance.SetDeleteArray(&deleteArray_GenParticle);
      instance.SetDestructor(&destruct_GenParticle);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GenParticle*)
   {
      return GenerateInitInstanceLocal((::GenParticle*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GenParticle*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *GenParticle_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::GenParticle*)0x0)->GetClass();
      GenParticle_TClassManip(theClass);
   return theClass;
   }

   static void GenParticle_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Tau_Dictionary();
   static void Tau_TClassManip(TClass*);
   static void *new_Tau(void *p = 0);
   static void *newArray_Tau(Long_t size, void *p);
   static void delete_Tau(void *p);
   static void deleteArray_Tau(void *p);
   static void destruct_Tau(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Tau*)
   {
      ::Tau *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Tau));
      static ::ROOT::TGenericClassInfo 
         instance("Tau", "Tau.hpp", 11,
                  typeid(::Tau), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Tau_Dictionary, isa_proxy, 4,
                  sizeof(::Tau) );
      instance.SetNew(&new_Tau);
      instance.SetNewArray(&newArray_Tau);
      instance.SetDelete(&delete_Tau);
      instance.SetDeleteArray(&deleteArray_Tau);
      instance.SetDestructor(&destruct_Tau);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Tau*)
   {
      return GenerateInitInstanceLocal((::Tau*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Tau*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Tau_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Tau*)0x0)->GetClass();
      Tau_TClassManip(theClass);
   return theClass;
   }

   static void Tau_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Met_Dictionary();
   static void Met_TClassManip(TClass*);
   static void *new_Met(void *p = 0);
   static void *newArray_Met(Long_t size, void *p);
   static void delete_Met(void *p);
   static void deleteArray_Met(void *p);
   static void destruct_Met(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Met*)
   {
      ::Met *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Met));
      static ::ROOT::TGenericClassInfo 
         instance("Met", "Met.hpp", 9,
                  typeid(::Met), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Met_Dictionary, isa_proxy, 4,
                  sizeof(::Met) );
      instance.SetNew(&new_Met);
      instance.SetNewArray(&newArray_Met);
      instance.SetDelete(&delete_Met);
      instance.SetDeleteArray(&deleteArray_Met);
      instance.SetDestructor(&destruct_Met);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Met*)
   {
      return GenerateInitInstanceLocal((::Met*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Met*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Met_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Met*)0x0)->GetClass();
      Met_TClassManip(theClass);
   return theClass;
   }

   static void Met_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_Dictionary();
   static void SF_TClassManip(TClass*);
   static void *new_SF(void *p = 0);
   static void *newArray_SF(Long_t size, void *p);
   static void delete_SF(void *p);
   static void deleteArray_SF(void *p);
   static void destruct_SF(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF*)
   {
      ::SF *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF));
      static ::ROOT::TGenericClassInfo 
         instance("SF", "SF.hpp", 12,
                  typeid(::SF), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_Dictionary, isa_proxy, 4,
                  sizeof(::SF) );
      instance.SetNew(&new_SF);
      instance.SetNewArray(&newArray_SF);
      instance.SetDelete(&delete_SF);
      instance.SetDeleteArray(&deleteArray_SF);
      instance.SetDestructor(&destruct_SF);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF*)
   {
      return GenerateInitInstanceLocal((::SF*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF*)0x0)->GetClass();
      SF_TClassManip(theClass);
   return theClass;
   }

   static void SF_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_Asymm_Dictionary();
   static void SF_Asymm_TClassManip(TClass*);
   static void *new_SF_Asymm(void *p = 0);
   static void *newArray_SF_Asymm(Long_t size, void *p);
   static void delete_SF_Asymm(void *p);
   static void deleteArray_SF_Asymm(void *p);
   static void destruct_SF_Asymm(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_Asymm*)
   {
      ::SF_Asymm *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_Asymm));
      static ::ROOT::TGenericClassInfo 
         instance("SF_Asymm", "SF.hpp", 41,
                  typeid(::SF_Asymm), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_Asymm_Dictionary, isa_proxy, 4,
                  sizeof(::SF_Asymm) );
      instance.SetNew(&new_SF_Asymm);
      instance.SetNewArray(&newArray_SF_Asymm);
      instance.SetDelete(&delete_SF_Asymm);
      instance.SetDeleteArray(&deleteArray_SF_Asymm);
      instance.SetDestructor(&destruct_SF_Asymm);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_Asymm*)
   {
      return GenerateInitInstanceLocal((::SF_Asymm*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_Asymm*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_Asymm_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_Asymm*)0x0)->GetClass();
      SF_Asymm_TClassManip(theClass);
   return theClass;
   }

   static void SF_Asymm_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_PtEta_Dictionary();
   static void SF_PtEta_TClassManip(TClass*);
   static void *new_SF_PtEta(void *p = 0);
   static void *newArray_SF_PtEta(Long_t size, void *p);
   static void delete_SF_PtEta(void *p);
   static void deleteArray_SF_PtEta(void *p);
   static void destruct_SF_PtEta(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_PtEta*)
   {
      ::SF_PtEta *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_PtEta));
      static ::ROOT::TGenericClassInfo 
         instance("SF_PtEta", "SF.hpp", 50,
                  typeid(::SF_PtEta), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_PtEta_Dictionary, isa_proxy, 4,
                  sizeof(::SF_PtEta) );
      instance.SetNew(&new_SF_PtEta);
      instance.SetNewArray(&newArray_SF_PtEta);
      instance.SetDelete(&delete_SF_PtEta);
      instance.SetDeleteArray(&deleteArray_SF_PtEta);
      instance.SetDestructor(&destruct_SF_PtEta);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_PtEta*)
   {
      return GenerateInitInstanceLocal((::SF_PtEta*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_PtEta*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_PtEta_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_PtEta*)0x0)->GetClass();
      SF_PtEta_TClassManip(theClass);
   return theClass;
   }

   static void SF_PtEta_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_TH2F_Dictionary();
   static void SF_TH2F_TClassManip(TClass*);
   static void *new_SF_TH2F(void *p = 0);
   static void *newArray_SF_TH2F(Long_t size, void *p);
   static void delete_SF_TH2F(void *p);
   static void deleteArray_SF_TH2F(void *p);
   static void destruct_SF_TH2F(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_TH2F*)
   {
      ::SF_TH2F *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_TH2F));
      static ::ROOT::TGenericClassInfo 
         instance("SF_TH2F", "SF.hpp", 99,
                  typeid(::SF_TH2F), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_TH2F_Dictionary, isa_proxy, 4,
                  sizeof(::SF_TH2F) );
      instance.SetNew(&new_SF_TH2F);
      instance.SetNewArray(&newArray_SF_TH2F);
      instance.SetDelete(&delete_SF_TH2F);
      instance.SetDeleteArray(&deleteArray_SF_TH2F);
      instance.SetDestructor(&destruct_SF_TH2F);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_TH2F*)
   {
      return GenerateInitInstanceLocal((::SF_TH2F*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_TH2F*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_TH2F_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_TH2F*)0x0)->GetClass();
      SF_TH2F_TClassManip(theClass);
   return theClass;
   }

   static void SF_TH2F_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_TF1_Dictionary();
   static void SF_TF1_TClassManip(TClass*);
   static void *new_SF_TF1(void *p = 0);
   static void *newArray_SF_TF1(Long_t size, void *p);
   static void delete_SF_TF1(void *p);
   static void deleteArray_SF_TF1(void *p);
   static void destruct_SF_TF1(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_TF1*)
   {
      ::SF_TF1 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_TF1));
      static ::ROOT::TGenericClassInfo 
         instance("SF_TF1", "SF.hpp", 129,
                  typeid(::SF_TF1), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_TF1_Dictionary, isa_proxy, 4,
                  sizeof(::SF_TF1) );
      instance.SetNew(&new_SF_TF1);
      instance.SetNewArray(&newArray_SF_TF1);
      instance.SetDelete(&delete_SF_TF1);
      instance.SetDeleteArray(&deleteArray_SF_TF1);
      instance.SetDestructor(&destruct_SF_TF1);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_TF1*)
   {
      return GenerateInitInstanceLocal((::SF_TF1*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_TF1*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_TF1_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_TF1*)0x0)->GetClass();
      SF_TF1_TClassManip(theClass);
   return theClass;
   }

   static void SF_TF1_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_TF2_Dictionary();
   static void SF_TF2_TClassManip(TClass*);
   static void *new_SF_TF2(void *p = 0);
   static void *newArray_SF_TF2(Long_t size, void *p);
   static void delete_SF_TF2(void *p);
   static void deleteArray_SF_TF2(void *p);
   static void destruct_SF_TF2(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_TF2*)
   {
      ::SF_TF2 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_TF2));
      static ::ROOT::TGenericClassInfo 
         instance("SF_TF2", "SF.hpp", 151,
                  typeid(::SF_TF2), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_TF2_Dictionary, isa_proxy, 4,
                  sizeof(::SF_TF2) );
      instance.SetNew(&new_SF_TF2);
      instance.SetNewArray(&newArray_SF_TF2);
      instance.SetDelete(&delete_SF_TF2);
      instance.SetDeleteArray(&deleteArray_SF_TF2);
      instance.SetDestructor(&destruct_SF_TF2);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_TF2*)
   {
      return GenerateInitInstanceLocal((::SF_TF2*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_TF2*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_TF2_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_TF2*)0x0)->GetClass();
      SF_TF2_TClassManip(theClass);
   return theClass;
   }

   static void SF_TF2_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_PtSpline_Dictionary();
   static void SF_PtSpline_TClassManip(TClass*);
   static void *new_SF_PtSpline(void *p = 0);
   static void *newArray_SF_PtSpline(Long_t size, void *p);
   static void delete_SF_PtSpline(void *p);
   static void deleteArray_SF_PtSpline(void *p);
   static void destruct_SF_PtSpline(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_PtSpline*)
   {
      ::SF_PtSpline *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_PtSpline));
      static ::ROOT::TGenericClassInfo 
         instance("SF_PtSpline", "SF.hpp", 173,
                  typeid(::SF_PtSpline), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_PtSpline_Dictionary, isa_proxy, 4,
                  sizeof(::SF_PtSpline) );
      instance.SetNew(&new_SF_PtSpline);
      instance.SetNewArray(&newArray_SF_PtSpline);
      instance.SetDelete(&delete_SF_PtSpline);
      instance.SetDeleteArray(&deleteArray_SF_PtSpline);
      instance.SetDestructor(&destruct_SF_PtSpline);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_PtSpline*)
   {
      return GenerateInitInstanceLocal((::SF_PtSpline*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_PtSpline*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_PtSpline_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_PtSpline*)0x0)->GetClass();
      SF_PtSpline_TClassManip(theClass);
   return theClass;
   }

   static void SF_PtSpline_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SF_CSV_Dictionary();
   static void SF_CSV_TClassManip(TClass*);
   static void *new_SF_CSV(void *p = 0);
   static void *newArray_SF_CSV(Long_t size, void *p);
   static void delete_SF_CSV(void *p);
   static void deleteArray_SF_CSV(void *p);
   static void destruct_SF_CSV(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SF_CSV*)
   {
      ::SF_CSV *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SF_CSV));
      static ::ROOT::TGenericClassInfo 
         instance("SF_CSV", "SF.hpp", 198,
                  typeid(::SF_CSV), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SF_CSV_Dictionary, isa_proxy, 4,
                  sizeof(::SF_CSV) );
      instance.SetNew(&new_SF_CSV);
      instance.SetNewArray(&newArray_SF_CSV);
      instance.SetDelete(&delete_SF_CSV);
      instance.SetDeleteArray(&deleteArray_SF_CSV);
      instance.SetDestructor(&destruct_SF_CSV);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SF_CSV*)
   {
      return GenerateInitInstanceLocal((::SF_CSV*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SF_CSV*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SF_CSV_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SF_CSV*)0x0)->GetClass();
      SF_CSV_TClassManip(theClass);
   return theClass;
   }

   static void SF_CSV_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MC_Dictionary();
   static void MC_TClassManip(TClass*);
   static void *new_MC(void *p = 0);
   static void *newArray_MC(Long_t size, void *p);
   static void delete_MC(void *p);
   static void deleteArray_MC(void *p);
   static void destruct_MC(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MC*)
   {
      ::MC *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MC));
      static ::ROOT::TGenericClassInfo 
         instance("MC", "MC.hpp", 11,
                  typeid(::MC), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MC_Dictionary, isa_proxy, 4,
                  sizeof(::MC) );
      instance.SetNew(&new_MC);
      instance.SetNewArray(&newArray_MC);
      instance.SetDelete(&delete_MC);
      instance.SetDeleteArray(&deleteArray_MC);
      instance.SetDestructor(&destruct_MC);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MC*)
   {
      return GenerateInitInstanceLocal((::MC*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MC*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MC_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MC*)0x0)->GetClass();
      MC_TClassManip(theClass);
   return theClass;
   }

   static void MC_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Weight_Dictionary();
   static void Weight_TClassManip(TClass*);
   static void *new_Weight(void *p = 0);
   static void *newArray_Weight(Long_t size, void *p);
   static void delete_Weight(void *p);
   static void deleteArray_Weight(void *p);
   static void destruct_Weight(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Weight*)
   {
      ::Weight *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Weight));
      static ::ROOT::TGenericClassInfo 
         instance("Weight", "Weight.hpp", 13,
                  typeid(::Weight), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Weight_Dictionary, isa_proxy, 4,
                  sizeof(::Weight) );
      instance.SetNew(&new_Weight);
      instance.SetNewArray(&newArray_Weight);
      instance.SetDelete(&delete_Weight);
      instance.SetDeleteArray(&deleteArray_Weight);
      instance.SetDestructor(&destruct_Weight);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Weight*)
   {
      return GenerateInitInstanceLocal((::Weight*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Weight*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Weight_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Weight*)0x0)->GetClass();
      Weight_TClassManip(theClass);
   return theClass;
   }

   static void Weight_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Event_Dictionary();
   static void Event_TClassManip(TClass*);
   static void *new_Event(void *p = 0);
   static void *newArray_Event(Long_t size, void *p);
   static void delete_Event(void *p);
   static void deleteArray_Event(void *p);
   static void destruct_Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Event*)
   {
      ::Event *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Event));
      static ::ROOT::TGenericClassInfo 
         instance("Event", "Event.hpp", 33,
                  typeid(::Event), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Event_Dictionary, isa_proxy, 4,
                  sizeof(::Event) );
      instance.SetNew(&new_Event);
      instance.SetNewArray(&newArray_Event);
      instance.SetDelete(&delete_Event);
      instance.SetDeleteArray(&deleteArray_Event);
      instance.SetDestructor(&destruct_Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Event*)
   {
      return GenerateInitInstanceLocal((::Event*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Event*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Event_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetClass();
      Event_TClassManip(theClass);
   return theClass;
   }

   static void Event_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *CutSelector_Dictionary();
   static void CutSelector_TClassManip(TClass*);
   static void *new_CutSelector(void *p = 0);
   static void *newArray_CutSelector(Long_t size, void *p);
   static void delete_CutSelector(void *p);
   static void deleteArray_CutSelector(void *p);
   static void destruct_CutSelector(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CutSelector*)
   {
      ::CutSelector *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::CutSelector));
      static ::ROOT::TGenericClassInfo 
         instance("CutSelector", "CutSelector.hpp", 7,
                  typeid(::CutSelector), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &CutSelector_Dictionary, isa_proxy, 4,
                  sizeof(::CutSelector) );
      instance.SetNew(&new_CutSelector);
      instance.SetNewArray(&newArray_CutSelector);
      instance.SetDelete(&delete_CutSelector);
      instance.SetDeleteArray(&deleteArray_CutSelector);
      instance.SetDestructor(&destruct_CutSelector);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CutSelector*)
   {
      return GenerateInitInstanceLocal((::CutSelector*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::CutSelector*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *CutSelector_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::CutSelector*)0x0)->GetClass();
      CutSelector_TClassManip(theClass);
   return theClass;
   }

   static void CutSelector_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BaseFitter_Dictionary();
   static void BaseFitter_TClassManip(TClass*);
   static void delete_BaseFitter(void *p);
   static void deleteArray_BaseFitter(void *p);
   static void destruct_BaseFitter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BaseFitter*)
   {
      ::BaseFitter *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BaseFitter));
      static ::ROOT::TGenericClassInfo 
         instance("BaseFitter", "BaseFitter.hpp", 5,
                  typeid(::BaseFitter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &BaseFitter_Dictionary, isa_proxy, 4,
                  sizeof(::BaseFitter) );
      instance.SetDelete(&delete_BaseFitter);
      instance.SetDeleteArray(&deleteArray_BaseFitter);
      instance.SetDestructor(&destruct_BaseFitter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BaseFitter*)
   {
      return GenerateInitInstanceLocal((::BaseFitter*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::BaseFitter*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BaseFitter_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BaseFitter*)0x0)->GetClass();
      BaseFitter_TClassManip(theClass);
   return theClass;
   }

   static void BaseFitter_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Fitter_Dictionary();
   static void Fitter_TClassManip(TClass*);
   static void *new_Fitter(void *p = 0);
   static void *newArray_Fitter(Long_t size, void *p);
   static void delete_Fitter(void *p);
   static void deleteArray_Fitter(void *p);
   static void destruct_Fitter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Fitter*)
   {
      ::Fitter *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Fitter));
      static ::ROOT::TGenericClassInfo 
         instance("Fitter", "Fitter.hpp", 39,
                  typeid(::Fitter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Fitter_Dictionary, isa_proxy, 4,
                  sizeof(::Fitter) );
      instance.SetNew(&new_Fitter);
      instance.SetNewArray(&newArray_Fitter);
      instance.SetDelete(&delete_Fitter);
      instance.SetDeleteArray(&deleteArray_Fitter);
      instance.SetDestructor(&destruct_Fitter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Fitter*)
   {
      return GenerateInitInstanceLocal((::Fitter*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Fitter*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Fitter_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Fitter*)0x0)->GetClass();
      Fitter_TClassManip(theClass);
   return theClass;
   }

   static void Fitter_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BackgroundFitter_Dictionary();
   static void BackgroundFitter_TClassManip(TClass*);
   static void *new_BackgroundFitter(void *p = 0);
   static void *newArray_BackgroundFitter(Long_t size, void *p);
   static void delete_BackgroundFitter(void *p);
   static void deleteArray_BackgroundFitter(void *p);
   static void destruct_BackgroundFitter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BackgroundFitter*)
   {
      ::BackgroundFitter *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BackgroundFitter));
      static ::ROOT::TGenericClassInfo 
         instance("BackgroundFitter", "BackgroundFitter.hpp", 43,
                  typeid(::BackgroundFitter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &BackgroundFitter_Dictionary, isa_proxy, 4,
                  sizeof(::BackgroundFitter) );
      instance.SetNew(&new_BackgroundFitter);
      instance.SetNewArray(&newArray_BackgroundFitter);
      instance.SetDelete(&delete_BackgroundFitter);
      instance.SetDeleteArray(&deleteArray_BackgroundFitter);
      instance.SetDestructor(&destruct_BackgroundFitter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BackgroundFitter*)
   {
      return GenerateInitInstanceLocal((::BackgroundFitter*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::BackgroundFitter*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BackgroundFitter_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BackgroundFitter*)0x0)->GetClass();
      BackgroundFitter_TClassManip(theClass);
   return theClass;
   }

   static void BackgroundFitter_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearBase_Dictionary();
   static void SmearBase_TClassManip(TClass*);
   static void *new_SmearBase(void *p = 0);
   static void *newArray_SmearBase(Long_t size, void *p);
   static void delete_SmearBase(void *p);
   static void deleteArray_SmearBase(void *p);
   static void destruct_SmearBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearBase*)
   {
      ::SmearBase *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearBase));
      static ::ROOT::TGenericClassInfo 
         instance("SmearBase", "Smearer.hpp", 15,
                  typeid(::SmearBase), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearBase_Dictionary, isa_proxy, 4,
                  sizeof(::SmearBase) );
      instance.SetNew(&new_SmearBase);
      instance.SetNewArray(&newArray_SmearBase);
      instance.SetDelete(&delete_SmearBase);
      instance.SetDeleteArray(&deleteArray_SmearBase);
      instance.SetDestructor(&destruct_SmearBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearBase*)
   {
      return GenerateInitInstanceLocal((::SmearBase*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearBase*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearBase_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearBase*)0x0)->GetClass();
      SmearBase_TClassManip(theClass);
   return theClass;
   }

   static void SmearBase_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearJes_Dictionary();
   static void SmearJes_TClassManip(TClass*);
   static void *new_SmearJes(void *p = 0);
   static void *newArray_SmearJes(Long_t size, void *p);
   static void delete_SmearJes(void *p);
   static void deleteArray_SmearJes(void *p);
   static void destruct_SmearJes(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearJes*)
   {
      ::SmearJes *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearJes));
      static ::ROOT::TGenericClassInfo 
         instance("SmearJes", "Smearer.hpp", 46,
                  typeid(::SmearJes), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearJes_Dictionary, isa_proxy, 4,
                  sizeof(::SmearJes) );
      instance.SetNew(&new_SmearJes);
      instance.SetNewArray(&newArray_SmearJes);
      instance.SetDelete(&delete_SmearJes);
      instance.SetDeleteArray(&deleteArray_SmearJes);
      instance.SetDestructor(&destruct_SmearJes);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearJes*)
   {
      return GenerateInitInstanceLocal((::SmearJes*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearJes*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearJes_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearJes*)0x0)->GetClass();
      SmearJes_TClassManip(theClass);
   return theClass;
   }

   static void SmearJes_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearTauScale_Dictionary();
   static void SmearTauScale_TClassManip(TClass*);
   static void *new_SmearTauScale(void *p = 0);
   static void *newArray_SmearTauScale(Long_t size, void *p);
   static void delete_SmearTauScale(void *p);
   static void deleteArray_SmearTauScale(void *p);
   static void destruct_SmearTauScale(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearTauScale*)
   {
      ::SmearTauScale *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearTauScale));
      static ::ROOT::TGenericClassInfo 
         instance("SmearTauScale", "Smearer.hpp", 90,
                  typeid(::SmearTauScale), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearTauScale_Dictionary, isa_proxy, 4,
                  sizeof(::SmearTauScale) );
      instance.SetNew(&new_SmearTauScale);
      instance.SetNewArray(&newArray_SmearTauScale);
      instance.SetDelete(&delete_SmearTauScale);
      instance.SetDeleteArray(&deleteArray_SmearTauScale);
      instance.SetDestructor(&destruct_SmearTauScale);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearTauScale*)
   {
      return GenerateInitInstanceLocal((::SmearTauScale*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearTauScale*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearTauScale_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearTauScale*)0x0)->GetClass();
      SmearTauScale_TClassManip(theClass);
   return theClass;
   }

   static void SmearTauScale_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearJer_Dictionary();
   static void SmearJer_TClassManip(TClass*);
   static void *new_SmearJer(void *p = 0);
   static void *newArray_SmearJer(Long_t size, void *p);
   static void delete_SmearJer(void *p);
   static void deleteArray_SmearJer(void *p);
   static void destruct_SmearJer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearJer*)
   {
      ::SmearJer *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearJer));
      static ::ROOT::TGenericClassInfo 
         instance("SmearJer", "Smearer.hpp", 97,
                  typeid(::SmearJer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearJer_Dictionary, isa_proxy, 4,
                  sizeof(::SmearJer) );
      instance.SetNew(&new_SmearJer);
      instance.SetNewArray(&newArray_SmearJer);
      instance.SetDelete(&delete_SmearJer);
      instance.SetDeleteArray(&deleteArray_SmearJer);
      instance.SetDestructor(&destruct_SmearJer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearJer*)
   {
      return GenerateInitInstanceLocal((::SmearJer*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearJer*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearJer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearJer*)0x0)->GetClass();
      SmearJer_TClassManip(theClass);
   return theClass;
   }

   static void SmearJer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearUncluster_Dictionary();
   static void SmearUncluster_TClassManip(TClass*);
   static void *new_SmearUncluster(void *p = 0);
   static void *newArray_SmearUncluster(Long_t size, void *p);
   static void delete_SmearUncluster(void *p);
   static void deleteArray_SmearUncluster(void *p);
   static void destruct_SmearUncluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearUncluster*)
   {
      ::SmearUncluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearUncluster));
      static ::ROOT::TGenericClassInfo 
         instance("SmearUncluster", "Smearer.hpp", 105,
                  typeid(::SmearUncluster), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearUncluster_Dictionary, isa_proxy, 4,
                  sizeof(::SmearUncluster) );
      instance.SetNew(&new_SmearUncluster);
      instance.SetNewArray(&newArray_SmearUncluster);
      instance.SetDelete(&delete_SmearUncluster);
      instance.SetDeleteArray(&deleteArray_SmearUncluster);
      instance.SetDestructor(&destruct_SmearUncluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearUncluster*)
   {
      return GenerateInitInstanceLocal((::SmearUncluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearUncluster*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearUncluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearUncluster*)0x0)->GetClass();
      SmearUncluster_TClassManip(theClass);
   return theClass;
   }

   static void SmearUncluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearSF_Dictionary();
   static void SmearSF_TClassManip(TClass*);
   static void *new_SmearSF(void *p = 0);
   static void *newArray_SmearSF(Long_t size, void *p);
   static void delete_SmearSF(void *p);
   static void deleteArray_SmearSF(void *p);
   static void destruct_SmearSF(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearSF*)
   {
      ::SmearSF *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearSF));
      static ::ROOT::TGenericClassInfo 
         instance("SmearSF", "Smearer.hpp", 112,
                  typeid(::SmearSF), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearSF_Dictionary, isa_proxy, 4,
                  sizeof(::SmearSF) );
      instance.SetNew(&new_SmearSF);
      instance.SetNewArray(&newArray_SmearSF);
      instance.SetDelete(&delete_SmearSF);
      instance.SetDeleteArray(&deleteArray_SmearSF);
      instance.SetDestructor(&destruct_SmearSF);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearSF*)
   {
      return GenerateInitInstanceLocal((::SmearSF*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearSF*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearSF_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearSF*)0x0)->GetClass();
      SmearSF_TClassManip(theClass);
   return theClass;
   }

   static void SmearSF_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearPu_Dictionary();
   static void SmearPu_TClassManip(TClass*);
   static void *new_SmearPu(void *p = 0);
   static void *newArray_SmearPu(Long_t size, void *p);
   static void delete_SmearPu(void *p);
   static void deleteArray_SmearPu(void *p);
   static void destruct_SmearPu(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearPu*)
   {
      ::SmearPu *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearPu));
      static ::ROOT::TGenericClassInfo 
         instance("SmearPu", "Smearer.hpp", 121,
                  typeid(::SmearPu), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearPu_Dictionary, isa_proxy, 4,
                  sizeof(::SmearPu) );
      instance.SetNew(&new_SmearPu);
      instance.SetNewArray(&newArray_SmearPu);
      instance.SetDelete(&delete_SmearPu);
      instance.SetDeleteArray(&deleteArray_SmearPu);
      instance.SetDestructor(&destruct_SmearPu);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearPu*)
   {
      return GenerateInitInstanceLocal((::SmearPu*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearPu*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearPu_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearPu*)0x0)->GetClass();
      SmearPu_TClassManip(theClass);
   return theClass;
   }

   static void SmearPu_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearScales_Dictionary();
   static void SmearScales_TClassManip(TClass*);
   static void *new_SmearScales(void *p = 0);
   static void *newArray_SmearScales(Long_t size, void *p);
   static void delete_SmearScales(void *p);
   static void deleteArray_SmearScales(void *p);
   static void destruct_SmearScales(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearScales*)
   {
      ::SmearScales *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearScales));
      static ::ROOT::TGenericClassInfo 
         instance("SmearScales", "Smearer.hpp", 128,
                  typeid(::SmearScales), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearScales_Dictionary, isa_proxy, 4,
                  sizeof(::SmearScales) );
      instance.SetNew(&new_SmearScales);
      instance.SetNewArray(&newArray_SmearScales);
      instance.SetDelete(&delete_SmearScales);
      instance.SetDeleteArray(&deleteArray_SmearScales);
      instance.SetDestructor(&destruct_SmearScales);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearScales*)
   {
      return GenerateInitInstanceLocal((::SmearScales*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearScales*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearScales_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearScales*)0x0)->GetClass();
      SmearScales_TClassManip(theClass);
   return theClass;
   }

   static void SmearScales_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearPdfs_Dictionary();
   static void SmearPdfs_TClassManip(TClass*);
   static void *new_SmearPdfs(void *p = 0);
   static void *newArray_SmearPdfs(Long_t size, void *p);
   static void delete_SmearPdfs(void *p);
   static void deleteArray_SmearPdfs(void *p);
   static void destruct_SmearPdfs(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearPdfs*)
   {
      ::SmearPdfs *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearPdfs));
      static ::ROOT::TGenericClassInfo 
         instance("SmearPdfs", "Smearer.hpp", 143,
                  typeid(::SmearPdfs), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearPdfs_Dictionary, isa_proxy, 4,
                  sizeof(::SmearPdfs) );
      instance.SetNew(&new_SmearPdfs);
      instance.SetNewArray(&newArray_SmearPdfs);
      instance.SetDelete(&delete_SmearPdfs);
      instance.SetDeleteArray(&deleteArray_SmearPdfs);
      instance.SetDestructor(&destruct_SmearPdfs);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearPdfs*)
   {
      return GenerateInitInstanceLocal((::SmearPdfs*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearPdfs*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearPdfs_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearPdfs*)0x0)->GetClass();
      SmearPdfs_TClassManip(theClass);
   return theClass;
   }

   static void SmearPdfs_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearBjets_Dictionary();
   static void SmearBjets_TClassManip(TClass*);
   static void *new_SmearBjets(void *p = 0);
   static void *newArray_SmearBjets(Long_t size, void *p);
   static void delete_SmearBjets(void *p);
   static void deleteArray_SmearBjets(void *p);
   static void destruct_SmearBjets(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearBjets*)
   {
      ::SmearBjets *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearBjets));
      static ::ROOT::TGenericClassInfo 
         instance("SmearBjets", "Smearer.hpp", 158,
                  typeid(::SmearBjets), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearBjets_Dictionary, isa_proxy, 4,
                  sizeof(::SmearBjets) );
      instance.SetNew(&new_SmearBjets);
      instance.SetNewArray(&newArray_SmearBjets);
      instance.SetDelete(&delete_SmearBjets);
      instance.SetDeleteArray(&deleteArray_SmearBjets);
      instance.SetDestructor(&destruct_SmearBjets);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearBjets*)
   {
      return GenerateInitInstanceLocal((::SmearBjets*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearBjets*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearBjets_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearBjets*)0x0)->GetClass();
      SmearBjets_TClassManip(theClass);
   return theClass;
   }

   static void SmearBjets_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearKaMuCa_Dictionary();
   static void SmearKaMuCa_TClassManip(TClass*);
   static void *new_SmearKaMuCa(void *p = 0);
   static void *newArray_SmearKaMuCa(Long_t size, void *p);
   static void delete_SmearKaMuCa(void *p);
   static void deleteArray_SmearKaMuCa(void *p);
   static void destruct_SmearKaMuCa(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearKaMuCa*)
   {
      ::SmearKaMuCa *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearKaMuCa));
      static ::ROOT::TGenericClassInfo 
         instance("SmearKaMuCa", "KaMuCa.hpp", 58,
                  typeid(::SmearKaMuCa), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearKaMuCa_Dictionary, isa_proxy, 4,
                  sizeof(::SmearKaMuCa) );
      instance.SetNew(&new_SmearKaMuCa);
      instance.SetNewArray(&newArray_SmearKaMuCa);
      instance.SetDelete(&delete_SmearKaMuCa);
      instance.SetDeleteArray(&deleteArray_SmearKaMuCa);
      instance.SetDestructor(&destruct_SmearKaMuCa);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearKaMuCa*)
   {
      return GenerateInitInstanceLocal((::SmearKaMuCa*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearKaMuCa*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearKaMuCa_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearKaMuCa*)0x0)->GetClass();
      SmearKaMuCa_TClassManip(theClass);
   return theClass;
   }

   static void SmearKaMuCa_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Looper_Dictionary();
   static void Looper_TClassManip(TClass*);
   static void *new_Looper(void *p = 0);
   static void *newArray_Looper(Long_t size, void *p);
   static void delete_Looper(void *p);
   static void deleteArray_Looper(void *p);
   static void destruct_Looper(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Looper*)
   {
      ::Looper *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Looper));
      static ::ROOT::TGenericClassInfo 
         instance("Looper", "Loop.hpp", 33,
                  typeid(::Looper), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Looper_Dictionary, isa_proxy, 4,
                  sizeof(::Looper) );
      instance.SetNew(&new_Looper);
      instance.SetNewArray(&newArray_Looper);
      instance.SetDelete(&delete_Looper);
      instance.SetDeleteArray(&deleteArray_Looper);
      instance.SetDestructor(&destruct_Looper);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Looper*)
   {
      return GenerateInitInstanceLocal((::Looper*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Looper*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Looper_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Looper*)0x0)->GetClass();
      Looper_TClassManip(theClass);
   return theClass;
   }

   static void Looper_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *PurityFit_Dictionary();
   static void PurityFit_TClassManip(TClass*);
   static void *new_PurityFit(void *p = 0);
   static void *newArray_PurityFit(Long_t size, void *p);
   static void delete_PurityFit(void *p);
   static void deleteArray_PurityFit(void *p);
   static void destruct_PurityFit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PurityFit*)
   {
      ::PurityFit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PurityFit));
      static ::ROOT::TGenericClassInfo 
         instance("PurityFit", "PurityFit.hpp", 37,
                  typeid(::PurityFit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PurityFit_Dictionary, isa_proxy, 4,
                  sizeof(::PurityFit) );
      instance.SetNew(&new_PurityFit);
      instance.SetNewArray(&newArray_PurityFit);
      instance.SetDelete(&delete_PurityFit);
      instance.SetDeleteArray(&deleteArray_PurityFit);
      instance.SetDestructor(&destruct_PurityFit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PurityFit*)
   {
      return GenerateInitInstanceLocal((::PurityFit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PurityFit*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PurityFit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PurityFit*)0x0)->GetClass();
      PurityFit_TClassManip(theClass);
   return theClass;
   }

   static void PurityFit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *PurityFitAnalytic_Dictionary();
   static void PurityFitAnalytic_TClassManip(TClass*);
   static void *new_PurityFitAnalytic(void *p = 0);
   static void *newArray_PurityFitAnalytic(Long_t size, void *p);
   static void delete_PurityFitAnalytic(void *p);
   static void deleteArray_PurityFitAnalytic(void *p);
   static void destruct_PurityFitAnalytic(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PurityFitAnalytic*)
   {
      ::PurityFitAnalytic *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PurityFitAnalytic));
      static ::ROOT::TGenericClassInfo 
         instance("PurityFitAnalytic", "PurityFitAnalytic.hpp", 41,
                  typeid(::PurityFitAnalytic), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PurityFitAnalytic_Dictionary, isa_proxy, 4,
                  sizeof(::PurityFitAnalytic) );
      instance.SetNew(&new_PurityFitAnalytic);
      instance.SetNewArray(&newArray_PurityFitAnalytic);
      instance.SetDelete(&delete_PurityFitAnalytic);
      instance.SetDeleteArray(&deleteArray_PurityFitAnalytic);
      instance.SetDestructor(&destruct_PurityFitAnalytic);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PurityFitAnalytic*)
   {
      return GenerateInitInstanceLocal((::PurityFitAnalytic*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PurityFitAnalytic*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PurityFitAnalytic_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PurityFitAnalytic*)0x0)->GetClass();
      PurityFitAnalytic_TClassManip(theClass);
   return theClass;
   }

   static void PurityFitAnalytic_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static void *new_RooCMSShape(void *p = 0);
   static void *newArray_RooCMSShape(Long_t size, void *p);
   static void delete_RooCMSShape(void *p);
   static void deleteArray_RooCMSShape(void *p);
   static void destruct_RooCMSShape(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooCMSShape*)
   {
      ::RooCMSShape *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooCMSShape >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooCMSShape", ::RooCMSShape::Class_Version(), "RooCMSShape.hpp", 33,
                  typeid(::RooCMSShape), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooCMSShape::Dictionary, isa_proxy, 4,
                  sizeof(::RooCMSShape) );
      instance.SetNew(&new_RooCMSShape);
      instance.SetNewArray(&newArray_RooCMSShape);
      instance.SetDelete(&delete_RooCMSShape);
      instance.SetDeleteArray(&deleteArray_RooCMSShape);
      instance.SetDestructor(&destruct_RooCMSShape);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooCMSShape*)
   {
      return GenerateInitInstanceLocal((::RooCMSShape*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooCMSShape*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooVoigtianShape(void *p = 0);
   static void *newArray_RooVoigtianShape(Long_t size, void *p);
   static void delete_RooVoigtianShape(void *p);
   static void deleteArray_RooVoigtianShape(void *p);
   static void destruct_RooVoigtianShape(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooVoigtianShape*)
   {
      ::RooVoigtianShape *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooVoigtianShape >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooVoigtianShape", ::RooVoigtianShape::Class_Version(), "RooVoigtianShape.hpp", 24,
                  typeid(::RooVoigtianShape), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooVoigtianShape::Dictionary, isa_proxy, 4,
                  sizeof(::RooVoigtianShape) );
      instance.SetNew(&new_RooVoigtianShape);
      instance.SetNewArray(&newArray_RooVoigtianShape);
      instance.SetDelete(&delete_RooVoigtianShape);
      instance.SetDeleteArray(&deleteArray_RooVoigtianShape);
      instance.SetDestructor(&destruct_RooVoigtianShape);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooVoigtianShape*)
   {
      return GenerateInitInstanceLocal((::RooVoigtianShape*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooVoigtianShape*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static TClass *SmearJesSource_Dictionary();
   static void SmearJesSource_TClassManip(TClass*);
   static void *new_SmearJesSource(void *p = 0);
   static void *newArray_SmearJesSource(Long_t size, void *p);
   static void delete_SmearJesSource(void *p);
   static void deleteArray_SmearJesSource(void *p);
   static void destruct_SmearJesSource(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmearJesSource*)
   {
      ::SmearJesSource *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmearJesSource));
      static ::ROOT::TGenericClassInfo 
         instance("SmearJesSource", "SmearJesSources.hpp", 10,
                  typeid(::SmearJesSource), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmearJesSource_Dictionary, isa_proxy, 4,
                  sizeof(::SmearJesSource) );
      instance.SetNew(&new_SmearJesSource);
      instance.SetNewArray(&newArray_SmearJesSource);
      instance.SetDelete(&delete_SmearJesSource);
      instance.SetDeleteArray(&deleteArray_SmearJesSource);
      instance.SetDestructor(&destruct_SmearJesSource);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmearJesSource*)
   {
      return GenerateInitInstanceLocal((::SmearJesSource*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmearJesSource*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmearJesSource_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmearJesSource*)0x0)->GetClass();
      SmearJesSource_TClassManip(theClass);
   return theClass;
   }

   static void SmearJesSource_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static void *new_RooBinnedUncertainties(void *p = 0);
   static void *newArray_RooBinnedUncertainties(Long_t size, void *p);
   static void delete_RooBinnedUncertainties(void *p);
   static void deleteArray_RooBinnedUncertainties(void *p);
   static void destruct_RooBinnedUncertainties(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooBinnedUncertainties*)
   {
      ::RooBinnedUncertainties *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooBinnedUncertainties >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooBinnedUncertainties", ::RooBinnedUncertainties::Class_Version(), "RooBinnedUncertainties.hpp", 25,
                  typeid(::RooBinnedUncertainties), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooBinnedUncertainties::Dictionary, isa_proxy, 4,
                  sizeof(::RooBinnedUncertainties) );
      instance.SetNew(&new_RooBinnedUncertainties);
      instance.SetNewArray(&newArray_RooBinnedUncertainties);
      instance.SetDelete(&delete_RooBinnedUncertainties);
      instance.SetDeleteArray(&deleteArray_RooBinnedUncertainties);
      instance.SetDestructor(&destruct_RooBinnedUncertainties);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooBinnedUncertainties*)
   {
      return GenerateInitInstanceLocal((::RooBinnedUncertainties*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooBinnedUncertainties*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr RooCMSShape::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooCMSShape::Class_Name()
{
   return "RooCMSShape";
}

//______________________________________________________________________________
const char *RooCMSShape::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooCMSShape*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooCMSShape::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooCMSShape*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooCMSShape::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooCMSShape*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooCMSShape::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooCMSShape*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooVoigtianShape::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooVoigtianShape::Class_Name()
{
   return "RooVoigtianShape";
}

//______________________________________________________________________________
const char *RooVoigtianShape::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooVoigtianShape*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooVoigtianShape::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooVoigtianShape*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooVoigtianShape::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooVoigtianShape*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooVoigtianShape::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooVoigtianShape*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooBinnedUncertainties::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooBinnedUncertainties::Class_Name()
{
   return "RooBinnedUncertainties";
}

//______________________________________________________________________________
const char *RooBinnedUncertainties::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooBinnedUncertainties*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooBinnedUncertainties::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooBinnedUncertainties*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooBinnedUncertainties::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooBinnedUncertainties*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooBinnedUncertainties::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooBinnedUncertainties*)0x0)->GetClass(); }
   return fgIsA;
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Object(void *p) {
      return  p ? new(p) ::Object : new ::Object;
   }
   static void *newArray_Object(Long_t nElements, void *p) {
      return p ? new(p) ::Object[nElements] : new ::Object[nElements];
   }
   // Wrapper around operator delete
   static void delete_Object(void *p) {
      delete ((::Object*)p);
   }
   static void deleteArray_Object(void *p) {
      delete [] ((::Object*)p);
   }
   static void destruct_Object(void *p) {
      typedef ::Object current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Object

namespace ROOT {
   // Wrappers around operator new
   static void *new_Jet(void *p) {
      return  p ? new(p) ::Jet : new ::Jet;
   }
   static void *newArray_Jet(Long_t nElements, void *p) {
      return p ? new(p) ::Jet[nElements] : new ::Jet[nElements];
   }
   // Wrapper around operator delete
   static void delete_Jet(void *p) {
      delete ((::Jet*)p);
   }
   static void deleteArray_Jet(void *p) {
      delete [] ((::Jet*)p);
   }
   static void destruct_Jet(void *p) {
      typedef ::Jet current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Jet

namespace ROOT {
   // Wrappers around operator new
   static void *new_FatJet(void *p) {
      return  p ? new(p) ::FatJet : new ::FatJet;
   }
   static void *newArray_FatJet(Long_t nElements, void *p) {
      return p ? new(p) ::FatJet[nElements] : new ::FatJet[nElements];
   }
   // Wrapper around operator delete
   static void delete_FatJet(void *p) {
      delete ((::FatJet*)p);
   }
   static void deleteArray_FatJet(void *p) {
      delete [] ((::FatJet*)p);
   }
   static void destruct_FatJet(void *p) {
      typedef ::FatJet current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::FatJet

namespace ROOT {
   // Wrappers around operator new
   static void *new_Lepton(void *p) {
      return  p ? new(p) ::Lepton : new ::Lepton;
   }
   static void *newArray_Lepton(Long_t nElements, void *p) {
      return p ? new(p) ::Lepton[nElements] : new ::Lepton[nElements];
   }
   // Wrapper around operator delete
   static void delete_Lepton(void *p) {
      delete ((::Lepton*)p);
   }
   static void deleteArray_Lepton(void *p) {
      delete [] ((::Lepton*)p);
   }
   static void destruct_Lepton(void *p) {
      typedef ::Lepton current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Lepton

namespace ROOT {
   // Wrappers around operator new
   static void *new_GenParticle(void *p) {
      return  p ? new(p) ::GenParticle : new ::GenParticle;
   }
   static void *newArray_GenParticle(Long_t nElements, void *p) {
      return p ? new(p) ::GenParticle[nElements] : new ::GenParticle[nElements];
   }
   // Wrapper around operator delete
   static void delete_GenParticle(void *p) {
      delete ((::GenParticle*)p);
   }
   static void deleteArray_GenParticle(void *p) {
      delete [] ((::GenParticle*)p);
   }
   static void destruct_GenParticle(void *p) {
      typedef ::GenParticle current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GenParticle

namespace ROOT {
   // Wrappers around operator new
   static void *new_Tau(void *p) {
      return  p ? new(p) ::Tau : new ::Tau;
   }
   static void *newArray_Tau(Long_t nElements, void *p) {
      return p ? new(p) ::Tau[nElements] : new ::Tau[nElements];
   }
   // Wrapper around operator delete
   static void delete_Tau(void *p) {
      delete ((::Tau*)p);
   }
   static void deleteArray_Tau(void *p) {
      delete [] ((::Tau*)p);
   }
   static void destruct_Tau(void *p) {
      typedef ::Tau current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Tau

namespace ROOT {
   // Wrappers around operator new
   static void *new_Met(void *p) {
      return  p ? new(p) ::Met : new ::Met;
   }
   static void *newArray_Met(Long_t nElements, void *p) {
      return p ? new(p) ::Met[nElements] : new ::Met[nElements];
   }
   // Wrapper around operator delete
   static void delete_Met(void *p) {
      delete ((::Met*)p);
   }
   static void deleteArray_Met(void *p) {
      delete [] ((::Met*)p);
   }
   static void destruct_Met(void *p) {
      typedef ::Met current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Met

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF(void *p) {
      return  p ? new(p) ::SF : new ::SF;
   }
   static void *newArray_SF(Long_t nElements, void *p) {
      return p ? new(p) ::SF[nElements] : new ::SF[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF(void *p) {
      delete ((::SF*)p);
   }
   static void deleteArray_SF(void *p) {
      delete [] ((::SF*)p);
   }
   static void destruct_SF(void *p) {
      typedef ::SF current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_Asymm(void *p) {
      return  p ? new(p) ::SF_Asymm : new ::SF_Asymm;
   }
   static void *newArray_SF_Asymm(Long_t nElements, void *p) {
      return p ? new(p) ::SF_Asymm[nElements] : new ::SF_Asymm[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_Asymm(void *p) {
      delete ((::SF_Asymm*)p);
   }
   static void deleteArray_SF_Asymm(void *p) {
      delete [] ((::SF_Asymm*)p);
   }
   static void destruct_SF_Asymm(void *p) {
      typedef ::SF_Asymm current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_Asymm

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_PtEta(void *p) {
      return  p ? new(p) ::SF_PtEta : new ::SF_PtEta;
   }
   static void *newArray_SF_PtEta(Long_t nElements, void *p) {
      return p ? new(p) ::SF_PtEta[nElements] : new ::SF_PtEta[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_PtEta(void *p) {
      delete ((::SF_PtEta*)p);
   }
   static void deleteArray_SF_PtEta(void *p) {
      delete [] ((::SF_PtEta*)p);
   }
   static void destruct_SF_PtEta(void *p) {
      typedef ::SF_PtEta current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_PtEta

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_TH2F(void *p) {
      return  p ? new(p) ::SF_TH2F : new ::SF_TH2F;
   }
   static void *newArray_SF_TH2F(Long_t nElements, void *p) {
      return p ? new(p) ::SF_TH2F[nElements] : new ::SF_TH2F[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_TH2F(void *p) {
      delete ((::SF_TH2F*)p);
   }
   static void deleteArray_SF_TH2F(void *p) {
      delete [] ((::SF_TH2F*)p);
   }
   static void destruct_SF_TH2F(void *p) {
      typedef ::SF_TH2F current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_TH2F

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_TF1(void *p) {
      return  p ? new(p) ::SF_TF1 : new ::SF_TF1;
   }
   static void *newArray_SF_TF1(Long_t nElements, void *p) {
      return p ? new(p) ::SF_TF1[nElements] : new ::SF_TF1[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_TF1(void *p) {
      delete ((::SF_TF1*)p);
   }
   static void deleteArray_SF_TF1(void *p) {
      delete [] ((::SF_TF1*)p);
   }
   static void destruct_SF_TF1(void *p) {
      typedef ::SF_TF1 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_TF1

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_TF2(void *p) {
      return  p ? new(p) ::SF_TF2 : new ::SF_TF2;
   }
   static void *newArray_SF_TF2(Long_t nElements, void *p) {
      return p ? new(p) ::SF_TF2[nElements] : new ::SF_TF2[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_TF2(void *p) {
      delete ((::SF_TF2*)p);
   }
   static void deleteArray_SF_TF2(void *p) {
      delete [] ((::SF_TF2*)p);
   }
   static void destruct_SF_TF2(void *p) {
      typedef ::SF_TF2 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_TF2

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_PtSpline(void *p) {
      return  p ? new(p) ::SF_PtSpline : new ::SF_PtSpline;
   }
   static void *newArray_SF_PtSpline(Long_t nElements, void *p) {
      return p ? new(p) ::SF_PtSpline[nElements] : new ::SF_PtSpline[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_PtSpline(void *p) {
      delete ((::SF_PtSpline*)p);
   }
   static void deleteArray_SF_PtSpline(void *p) {
      delete [] ((::SF_PtSpline*)p);
   }
   static void destruct_SF_PtSpline(void *p) {
      typedef ::SF_PtSpline current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_PtSpline

namespace ROOT {
   // Wrappers around operator new
   static void *new_SF_CSV(void *p) {
      return  p ? new(p) ::SF_CSV : new ::SF_CSV;
   }
   static void *newArray_SF_CSV(Long_t nElements, void *p) {
      return p ? new(p) ::SF_CSV[nElements] : new ::SF_CSV[nElements];
   }
   // Wrapper around operator delete
   static void delete_SF_CSV(void *p) {
      delete ((::SF_CSV*)p);
   }
   static void deleteArray_SF_CSV(void *p) {
      delete [] ((::SF_CSV*)p);
   }
   static void destruct_SF_CSV(void *p) {
      typedef ::SF_CSV current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SF_CSV

namespace ROOT {
   // Wrappers around operator new
   static void *new_MC(void *p) {
      return  p ? new(p) ::MC : new ::MC;
   }
   static void *newArray_MC(Long_t nElements, void *p) {
      return p ? new(p) ::MC[nElements] : new ::MC[nElements];
   }
   // Wrapper around operator delete
   static void delete_MC(void *p) {
      delete ((::MC*)p);
   }
   static void deleteArray_MC(void *p) {
      delete [] ((::MC*)p);
   }
   static void destruct_MC(void *p) {
      typedef ::MC current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MC

namespace ROOT {
   // Wrappers around operator new
   static void *new_Weight(void *p) {
      return  p ? new(p) ::Weight : new ::Weight;
   }
   static void *newArray_Weight(Long_t nElements, void *p) {
      return p ? new(p) ::Weight[nElements] : new ::Weight[nElements];
   }
   // Wrapper around operator delete
   static void delete_Weight(void *p) {
      delete ((::Weight*)p);
   }
   static void deleteArray_Weight(void *p) {
      delete [] ((::Weight*)p);
   }
   static void destruct_Weight(void *p) {
      typedef ::Weight current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Weight

namespace ROOT {
   // Wrappers around operator new
   static void *new_Event(void *p) {
      return  p ? new(p) ::Event : new ::Event;
   }
   static void *newArray_Event(Long_t nElements, void *p) {
      return p ? new(p) ::Event[nElements] : new ::Event[nElements];
   }
   // Wrapper around operator delete
   static void delete_Event(void *p) {
      delete ((::Event*)p);
   }
   static void deleteArray_Event(void *p) {
      delete [] ((::Event*)p);
   }
   static void destruct_Event(void *p) {
      typedef ::Event current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Event

namespace ROOT {
   // Wrappers around operator new
   static void *new_CutSelector(void *p) {
      return  p ? new(p) ::CutSelector : new ::CutSelector;
   }
   static void *newArray_CutSelector(Long_t nElements, void *p) {
      return p ? new(p) ::CutSelector[nElements] : new ::CutSelector[nElements];
   }
   // Wrapper around operator delete
   static void delete_CutSelector(void *p) {
      delete ((::CutSelector*)p);
   }
   static void deleteArray_CutSelector(void *p) {
      delete [] ((::CutSelector*)p);
   }
   static void destruct_CutSelector(void *p) {
      typedef ::CutSelector current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CutSelector

namespace ROOT {
   // Wrapper around operator delete
   static void delete_BaseFitter(void *p) {
      delete ((::BaseFitter*)p);
   }
   static void deleteArray_BaseFitter(void *p) {
      delete [] ((::BaseFitter*)p);
   }
   static void destruct_BaseFitter(void *p) {
      typedef ::BaseFitter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BaseFitter

namespace ROOT {
   // Wrappers around operator new
   static void *new_Fitter(void *p) {
      return  p ? new(p) ::Fitter : new ::Fitter;
   }
   static void *newArray_Fitter(Long_t nElements, void *p) {
      return p ? new(p) ::Fitter[nElements] : new ::Fitter[nElements];
   }
   // Wrapper around operator delete
   static void delete_Fitter(void *p) {
      delete ((::Fitter*)p);
   }
   static void deleteArray_Fitter(void *p) {
      delete [] ((::Fitter*)p);
   }
   static void destruct_Fitter(void *p) {
      typedef ::Fitter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Fitter

namespace ROOT {
   // Wrappers around operator new
   static void *new_BackgroundFitter(void *p) {
      return  p ? new(p) ::BackgroundFitter : new ::BackgroundFitter;
   }
   static void *newArray_BackgroundFitter(Long_t nElements, void *p) {
      return p ? new(p) ::BackgroundFitter[nElements] : new ::BackgroundFitter[nElements];
   }
   // Wrapper around operator delete
   static void delete_BackgroundFitter(void *p) {
      delete ((::BackgroundFitter*)p);
   }
   static void deleteArray_BackgroundFitter(void *p) {
      delete [] ((::BackgroundFitter*)p);
   }
   static void destruct_BackgroundFitter(void *p) {
      typedef ::BackgroundFitter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BackgroundFitter

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearBase(void *p) {
      return  p ? new(p) ::SmearBase : new ::SmearBase;
   }
   static void *newArray_SmearBase(Long_t nElements, void *p) {
      return p ? new(p) ::SmearBase[nElements] : new ::SmearBase[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearBase(void *p) {
      delete ((::SmearBase*)p);
   }
   static void deleteArray_SmearBase(void *p) {
      delete [] ((::SmearBase*)p);
   }
   static void destruct_SmearBase(void *p) {
      typedef ::SmearBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearBase

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearJes(void *p) {
      return  p ? new(p) ::SmearJes : new ::SmearJes;
   }
   static void *newArray_SmearJes(Long_t nElements, void *p) {
      return p ? new(p) ::SmearJes[nElements] : new ::SmearJes[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearJes(void *p) {
      delete ((::SmearJes*)p);
   }
   static void deleteArray_SmearJes(void *p) {
      delete [] ((::SmearJes*)p);
   }
   static void destruct_SmearJes(void *p) {
      typedef ::SmearJes current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearJes

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearTauScale(void *p) {
      return  p ? new(p) ::SmearTauScale : new ::SmearTauScale;
   }
   static void *newArray_SmearTauScale(Long_t nElements, void *p) {
      return p ? new(p) ::SmearTauScale[nElements] : new ::SmearTauScale[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearTauScale(void *p) {
      delete ((::SmearTauScale*)p);
   }
   static void deleteArray_SmearTauScale(void *p) {
      delete [] ((::SmearTauScale*)p);
   }
   static void destruct_SmearTauScale(void *p) {
      typedef ::SmearTauScale current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearTauScale

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearJer(void *p) {
      return  p ? new(p) ::SmearJer : new ::SmearJer;
   }
   static void *newArray_SmearJer(Long_t nElements, void *p) {
      return p ? new(p) ::SmearJer[nElements] : new ::SmearJer[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearJer(void *p) {
      delete ((::SmearJer*)p);
   }
   static void deleteArray_SmearJer(void *p) {
      delete [] ((::SmearJer*)p);
   }
   static void destruct_SmearJer(void *p) {
      typedef ::SmearJer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearJer

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearUncluster(void *p) {
      return  p ? new(p) ::SmearUncluster : new ::SmearUncluster;
   }
   static void *newArray_SmearUncluster(Long_t nElements, void *p) {
      return p ? new(p) ::SmearUncluster[nElements] : new ::SmearUncluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearUncluster(void *p) {
      delete ((::SmearUncluster*)p);
   }
   static void deleteArray_SmearUncluster(void *p) {
      delete [] ((::SmearUncluster*)p);
   }
   static void destruct_SmearUncluster(void *p) {
      typedef ::SmearUncluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearUncluster

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearSF(void *p) {
      return  p ? new(p) ::SmearSF : new ::SmearSF;
   }
   static void *newArray_SmearSF(Long_t nElements, void *p) {
      return p ? new(p) ::SmearSF[nElements] : new ::SmearSF[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearSF(void *p) {
      delete ((::SmearSF*)p);
   }
   static void deleteArray_SmearSF(void *p) {
      delete [] ((::SmearSF*)p);
   }
   static void destruct_SmearSF(void *p) {
      typedef ::SmearSF current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearSF

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearPu(void *p) {
      return  p ? new(p) ::SmearPu : new ::SmearPu;
   }
   static void *newArray_SmearPu(Long_t nElements, void *p) {
      return p ? new(p) ::SmearPu[nElements] : new ::SmearPu[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearPu(void *p) {
      delete ((::SmearPu*)p);
   }
   static void deleteArray_SmearPu(void *p) {
      delete [] ((::SmearPu*)p);
   }
   static void destruct_SmearPu(void *p) {
      typedef ::SmearPu current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearPu

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearScales(void *p) {
      return  p ? new(p) ::SmearScales : new ::SmearScales;
   }
   static void *newArray_SmearScales(Long_t nElements, void *p) {
      return p ? new(p) ::SmearScales[nElements] : new ::SmearScales[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearScales(void *p) {
      delete ((::SmearScales*)p);
   }
   static void deleteArray_SmearScales(void *p) {
      delete [] ((::SmearScales*)p);
   }
   static void destruct_SmearScales(void *p) {
      typedef ::SmearScales current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearScales

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearPdfs(void *p) {
      return  p ? new(p) ::SmearPdfs : new ::SmearPdfs;
   }
   static void *newArray_SmearPdfs(Long_t nElements, void *p) {
      return p ? new(p) ::SmearPdfs[nElements] : new ::SmearPdfs[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearPdfs(void *p) {
      delete ((::SmearPdfs*)p);
   }
   static void deleteArray_SmearPdfs(void *p) {
      delete [] ((::SmearPdfs*)p);
   }
   static void destruct_SmearPdfs(void *p) {
      typedef ::SmearPdfs current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearPdfs

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearBjets(void *p) {
      return  p ? new(p) ::SmearBjets : new ::SmearBjets;
   }
   static void *newArray_SmearBjets(Long_t nElements, void *p) {
      return p ? new(p) ::SmearBjets[nElements] : new ::SmearBjets[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearBjets(void *p) {
      delete ((::SmearBjets*)p);
   }
   static void deleteArray_SmearBjets(void *p) {
      delete [] ((::SmearBjets*)p);
   }
   static void destruct_SmearBjets(void *p) {
      typedef ::SmearBjets current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearBjets

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearKaMuCa(void *p) {
      return  p ? new(p) ::SmearKaMuCa : new ::SmearKaMuCa;
   }
   static void *newArray_SmearKaMuCa(Long_t nElements, void *p) {
      return p ? new(p) ::SmearKaMuCa[nElements] : new ::SmearKaMuCa[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearKaMuCa(void *p) {
      delete ((::SmearKaMuCa*)p);
   }
   static void deleteArray_SmearKaMuCa(void *p) {
      delete [] ((::SmearKaMuCa*)p);
   }
   static void destruct_SmearKaMuCa(void *p) {
      typedef ::SmearKaMuCa current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearKaMuCa

namespace ROOT {
   // Wrappers around operator new
   static void *new_Looper(void *p) {
      return  p ? new(p) ::Looper : new ::Looper;
   }
   static void *newArray_Looper(Long_t nElements, void *p) {
      return p ? new(p) ::Looper[nElements] : new ::Looper[nElements];
   }
   // Wrapper around operator delete
   static void delete_Looper(void *p) {
      delete ((::Looper*)p);
   }
   static void deleteArray_Looper(void *p) {
      delete [] ((::Looper*)p);
   }
   static void destruct_Looper(void *p) {
      typedef ::Looper current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Looper

namespace ROOT {
   // Wrappers around operator new
   static void *new_PurityFit(void *p) {
      return  p ? new(p) ::PurityFit : new ::PurityFit;
   }
   static void *newArray_PurityFit(Long_t nElements, void *p) {
      return p ? new(p) ::PurityFit[nElements] : new ::PurityFit[nElements];
   }
   // Wrapper around operator delete
   static void delete_PurityFit(void *p) {
      delete ((::PurityFit*)p);
   }
   static void deleteArray_PurityFit(void *p) {
      delete [] ((::PurityFit*)p);
   }
   static void destruct_PurityFit(void *p) {
      typedef ::PurityFit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PurityFit

namespace ROOT {
   // Wrappers around operator new
   static void *new_PurityFitAnalytic(void *p) {
      return  p ? new(p) ::PurityFitAnalytic : new ::PurityFitAnalytic;
   }
   static void *newArray_PurityFitAnalytic(Long_t nElements, void *p) {
      return p ? new(p) ::PurityFitAnalytic[nElements] : new ::PurityFitAnalytic[nElements];
   }
   // Wrapper around operator delete
   static void delete_PurityFitAnalytic(void *p) {
      delete ((::PurityFitAnalytic*)p);
   }
   static void deleteArray_PurityFitAnalytic(void *p) {
      delete [] ((::PurityFitAnalytic*)p);
   }
   static void destruct_PurityFitAnalytic(void *p) {
      typedef ::PurityFitAnalytic current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PurityFitAnalytic

//______________________________________________________________________________
void RooCMSShape::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooCMSShape.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooCMSShape::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooCMSShape::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooCMSShape(void *p) {
      return  p ? new(p) ::RooCMSShape : new ::RooCMSShape;
   }
   static void *newArray_RooCMSShape(Long_t nElements, void *p) {
      return p ? new(p) ::RooCMSShape[nElements] : new ::RooCMSShape[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooCMSShape(void *p) {
      delete ((::RooCMSShape*)p);
   }
   static void deleteArray_RooCMSShape(void *p) {
      delete [] ((::RooCMSShape*)p);
   }
   static void destruct_RooCMSShape(void *p) {
      typedef ::RooCMSShape current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooCMSShape

//______________________________________________________________________________
void RooVoigtianShape::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooVoigtianShape.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooVoigtianShape::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooVoigtianShape::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooVoigtianShape(void *p) {
      return  p ? new(p) ::RooVoigtianShape : new ::RooVoigtianShape;
   }
   static void *newArray_RooVoigtianShape(Long_t nElements, void *p) {
      return p ? new(p) ::RooVoigtianShape[nElements] : new ::RooVoigtianShape[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooVoigtianShape(void *p) {
      delete ((::RooVoigtianShape*)p);
   }
   static void deleteArray_RooVoigtianShape(void *p) {
      delete [] ((::RooVoigtianShape*)p);
   }
   static void destruct_RooVoigtianShape(void *p) {
      typedef ::RooVoigtianShape current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooVoigtianShape

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmearJesSource(void *p) {
      return  p ? new(p) ::SmearJesSource : new ::SmearJesSource;
   }
   static void *newArray_SmearJesSource(Long_t nElements, void *p) {
      return p ? new(p) ::SmearJesSource[nElements] : new ::SmearJesSource[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmearJesSource(void *p) {
      delete ((::SmearJesSource*)p);
   }
   static void deleteArray_SmearJesSource(void *p) {
      delete [] ((::SmearJesSource*)p);
   }
   static void destruct_SmearJesSource(void *p) {
      typedef ::SmearJesSource current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmearJesSource

//______________________________________________________________________________
void RooBinnedUncertainties::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooBinnedUncertainties.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooBinnedUncertainties::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooBinnedUncertainties::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooBinnedUncertainties(void *p) {
      return  p ? new(p) ::RooBinnedUncertainties : new ::RooBinnedUncertainties;
   }
   static void *newArray_RooBinnedUncertainties(Long_t nElements, void *p) {
      return p ? new(p) ::RooBinnedUncertainties[nElements] : new ::RooBinnedUncertainties[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooBinnedUncertainties(void *p) {
      delete ((::RooBinnedUncertainties*)p);
   }
   static void deleteArray_RooBinnedUncertainties(void *p) {
      delete [] ((::RooBinnedUncertainties*)p);
   }
   static void destruct_RooBinnedUncertainties(void *p) {
      typedef ::RooBinnedUncertainties current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooBinnedUncertainties

namespace {
  void TriggerDictionaryInitialization_dict_Impl() {
    static const char* headers[] = {
0    };
    static const char* includePaths[] = {
"interface/",
"../",
"/cvmfs/cms.cern.ch/slc6_amd64_gcc630/lcg/root/6.10.08/include",
"/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/Object.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Object;
class __attribute__((annotate("$clingAutoload$interface/Jet.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Jet;
class __attribute__((annotate("$clingAutoload$interface/FatJet.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  FatJet;
class __attribute__((annotate("$clingAutoload$interface/Lepton.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Lepton;
class __attribute__((annotate("$clingAutoload$interface/GenParticle.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  GenParticle;
class __attribute__((annotate("$clingAutoload$interface/Tau.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Tau;
class __attribute__((annotate("$clingAutoload$interface/Met.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Met;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_Asymm;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_PtEta;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_TH2F;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_TF1;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_TF2;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_PtSpline;
class __attribute__((annotate("$clingAutoload$interface/SF.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  SF_CSV;
class __attribute__((annotate("$clingAutoload$interface/MC.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  MC;
class __attribute__((annotate("$clingAutoload$interface/Weight.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Weight;
class __attribute__((annotate("$clingAutoload$interface/Event.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisBase.hpp")))  Event;
class __attribute__((annotate("$clingAutoload$interface/CutSelector.hpp")))  __attribute__((annotate("$clingAutoload$interface/AnalysisChargedHiggsEWKPurity.hpp")))  CutSelector;
class __attribute__((annotate("$clingAutoload$interface/BaseFitter.hpp")))  BaseFitter;
class __attribute__((annotate("$clingAutoload$interface/Fitter.hpp")))  Fitter;
class __attribute__((annotate("$clingAutoload$interface/BackgroundFitter.hpp")))  BackgroundFitter;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearBase;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearJes;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearTauScale;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearJer;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearUncluster;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearSF;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearPu;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearScales;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearPdfs;
class __attribute__((annotate("$clingAutoload$interface/Smearer.hpp")))  __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearBjets;
class __attribute__((annotate("$clingAutoload$interface/KaMuCa.hpp")))  SmearKaMuCa;
class __attribute__((annotate("$clingAutoload$interface/Loop.hpp")))  Looper;
class __attribute__((annotate("$clingAutoload$interface/PurityFit.hpp")))  PurityFit;
class __attribute__((annotate("$clingAutoload$interface/PurityFitAnalytic.hpp")))  PurityFitAnalytic;
class __attribute__((annotate("$clingAutoload$interface/RooCMSShape.hpp")))  RooCMSShape;
class __attribute__((annotate(R"ATTRDUMP(Crystal Ball lineshape PDF)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$interface/RooVoigtianShape.hpp")))  RooVoigtianShape;
class __attribute__((annotate("$clingAutoload$interface/SmearJesSources.hpp")))  SmearJesSource;
class __attribute__((annotate("$clingAutoload$interface/RooBinnedUncertainties.hpp")))  RooBinnedUncertainties;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "interface/AnalysisBase.hpp"
#include "interface/AnalysisChargedHiggsEWKPurity.hpp"
#include "interface/AnalysisChargedHiggsHW.hpp"
#include "interface/AnalysisChargedHiggsMET.hpp"
#include "interface/AnalysisChargedHiggsQCDPurity.hpp"
#include "interface/AnalysisChargedHiggsSync.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include "interface/AnalysisChargedHiggsTmva.hpp"
#include "interface/AnalysisChargedHiggsTopBottom.hpp"
#include "interface/AnalysisChargedHiggsZW.hpp"
#include "interface/AnalysisChargedHiggsTBfullHad.hpp"
#include "interface/AnalysisDY.hpp"
#include "interface/AnalysisGG.hpp"
#include "interface/AnalysisHmumu.hpp"
#include "interface/AnalysisQG.hpp"
#include "interface/AnalysisScikit.hpp"
#include "interface/BaseFitter.hpp"
#include "interface/BTagCalibrationStandalone.hpp"
#include "interface/CatSelector.hpp"
#include "interface/Corrector.hpp"
#include "interface/CutSelector.hpp"
#include "interface/Davismt2.hpp"
#include "interface/Dumper.hpp"
#include "interface/EnergyScaleCorrection_class.hpp"
#include "interface/Event.hpp"
#include "interface/Fitter.hpp"
#include "interface/BackgroundFitter.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/GenParticle.hpp"
#include "interface/Handlers.hpp"
#include "interface/Jet.hpp"
#include "interface/FatJet.hpp"
#include "interface/JSON.hpp"
#include "interface/KalmanMuonCalibrator.hpp"
#include "interface/KaMuCa.hpp"
#include "interface/JetFilter2016.hpp"
#include "interface/Lepton.hpp"
#include "interface/LinkDef.hpp"
#include "interface/Loader.hpp"
#include "interface/Logger.hpp"
#include "interface/Loop.hpp"
#include "interface/MC.hpp"
#include "interface/Met.hpp"
#include "interface/MetFilters.hpp"
#include "interface/MetPhiCorrector.hpp"
#include "interface/Named.hpp"
#include "interface/Object.hpp"
#include "interface/Output.hpp"
#include "interface/Photon.hpp"
#include "interface/PreselectionAnalysis.hpp"
#include "interface/PU.hpp"
#include "interface/PurityFit.hpp"
#include "interface/PurityFitAnalytic.hpp"
#include "interface/PurityFitBase.hpp"
#include "interface/RegressionReader.hpp"
#include "interface/RegressionTrainer.hpp"
#include "interface/RoccoR.hpp"
#include "interface/Rochester.hpp"
#include "interface/RooCMSShape.hpp"
#include "interface/RooVoigtianShape.hpp"
#include "interface/ScaleAndSmear.hpp"
#include "interface/SF.hpp"
#include "interface/Smearable.hpp"
#include "interface/Smearer.hpp"
#include "interface/SmearJesSources.hpp"
#include "interface/SplitMC.hpp"
#include "interface/Tau.hpp"
#include "interface/TmvaTrainer.hpp"
#include "interface/Trigger.hpp"
#include "interface/Weight.hpp"
#include "interface/RooBinnedUncertainties.hpp"

// put template stuff here, MyClass<T> myclass;
//nampspace{
//	struct dictionary{
//	};
//};

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"BackgroundFitter", payloadCode, "@",
"BaseFitter", payloadCode, "@",
"CutSelector", payloadCode, "@",
"Event", payloadCode, "@",
"FatJet", payloadCode, "@",
"Fitter", payloadCode, "@",
"GenParticle", payloadCode, "@",
"Jet", payloadCode, "@",
"Lepton", payloadCode, "@",
"Looper", payloadCode, "@",
"MC", payloadCode, "@",
"Met", payloadCode, "@",
"Object", payloadCode, "@",
"PurityFit", payloadCode, "@",
"PurityFitAnalytic", payloadCode, "@",
"RooBinnedUncertainties", payloadCode, "@",
"RooCMSShape", payloadCode, "@",
"RooVoigtianShape", payloadCode, "@",
"SF", payloadCode, "@",
"SF_Asymm", payloadCode, "@",
"SF_CSV", payloadCode, "@",
"SF_PtEta", payloadCode, "@",
"SF_PtSpline", payloadCode, "@",
"SF_TF1", payloadCode, "@",
"SF_TF2", payloadCode, "@",
"SF_TH2F", payloadCode, "@",
"SmearBase", payloadCode, "@",
"SmearBjets", payloadCode, "@",
"SmearJer", payloadCode, "@",
"SmearJes", payloadCode, "@",
"SmearJesSource", payloadCode, "@",
"SmearKaMuCa", payloadCode, "@",
"SmearPdfs", payloadCode, "@",
"SmearPu", payloadCode, "@",
"SmearSF", payloadCode, "@",
"SmearScales", payloadCode, "@",
"SmearTauScale", payloadCode, "@",
"SmearUncluster", payloadCode, "@",
"Tau", payloadCode, "@",
"Weight", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_dict() {
  TriggerDictionaryInitialization_dict_Impl();
}
