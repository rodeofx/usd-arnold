// Copyright (c) 2019 Luma Pictures . All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. All advertising materials mentioning features or use of this software
// must display the following acknowledgement:
// This product includes software developed by the the organization.
//
// 4. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef USDAI_SHADER_EXPORT_H
#define USDAI_SHADER_EXPORT_H

#include "pxr/usd/usdAi/aiShader.h"

struct AtNode;
struct AtParamEntry;
class AtParamValue;
class AtArray;

PXR_NAMESPACE_OPEN_SCOPE

class AiShaderExport {
public:
    AiShaderExport(
        const UsdStagePtr& _stage, const SdfPath& parent_scope = SdfPath(),
        const UsdTimeCode& _time_code = UsdTimeCode::Default());
    virtual ~AiShaderExport() = default;
    void bind_material(const SdfPath& shader_path, const SdfPath& shape_path);
    SdfPath export_material(
        const char* material_name, AtNode* surf_shader,
        AtNode* disp_shader = nullptr);
    SdfPath export_arnold_node(
        const AtNode* arnold_node, const SdfPath& parent_path,
        const std::set<std::string>* exportable_params = nullptr);
    static void clean_arnold_name(std::string& name);
    bool get_output(
        const AtNode* src_arnold_node, UsdAiShader& src_shader,
        UsdShadeOutput& out, bool is_node_type = false,
        int32_t comp_index = -1);
    bool export_connection(
        const AtNode* dest_arnold_node, UsdAiShader& dest_shader,
        const std::string& dest_param_name,
        const std::string& dest_param_arnold_name, uint8_t arnold_param_type);
    bool export_connection(
        const AtNode* dest_arnold_node, UsdAiShader& dest_shader,
        const char* dest_param_name, const AtNode* src_arnold_node,
        UsdAiShader& src_shader, int32_t src_comp_index = -1);
    // bool export_connection(const AtNode* dest_arnold_node, UsdAiShader&
    // dest_shader,
    //                        const char* dest_param_name, uint8_t
    //                        arnold_param_type, const AtNode* src_arnold_node,
    //                        UsdAiShader& src_shader, int32_t
    //                        src_comp_index=-1);
    void export_parameter(
        const AtNode* arnold_node, UsdAiShader& shader,
        const char* arnold_param_name, uint8_t arnold_param_type, bool user);
    void collapse_shaders();

public:
    // Utility functions and definitions.
    struct ParamConversion {
        const SdfValueTypeName& type;
        std::function<VtValue(const AtNode*, const char*)> f;

        // TODO: see if move works in this case.
        ParamConversion(
            const SdfValueTypeName& _type,
            std::function<VtValue(const AtNode*, const char*)> _f)
            : type(_type), f(std::move(_f)) {}
    };

    static const ParamConversion* get_param_conversion(uint8_t type);

    struct DefaultValueConversion {
        const SdfValueTypeName& type;
        std::function<VtValue(const AtParamValue&, const AtParamEntry*)> f;

        // The signature is pretty heavy here. The reason for that is simple
        // when dealing with metadatas, we don't have a valid AtParamEntry
        // structure. So we have to make all the behavior optional, otherwise we
        // would need to duplicate functions.
        DefaultValueConversion(
            const SdfValueTypeName& _type,
            std::function<VtValue(const AtParamValue&, const AtParamEntry*)> _f)
            : type(_type), f(std::move(_f)) {}
    };

    static const DefaultValueConversion* get_default_value_conversion(
        uint8_t type);

    struct ArrayConversion {
        const SdfValueTypeName& type;
        std::function<VtValue(const AtArray*)> f;

        ArrayConversion(
            const SdfValueTypeName& _type,
            std::function<VtValue(const AtArray*)> _f)
            : type(_type), f(std::move(_f)) {}
    };

    static const ArrayConversion* get_array_conversion(uint8_t type);

protected:
    const UsdStagePtr m_stage;
    SdfPath m_shaders_scope;
    UsdTimeCode m_time_code;

private:
    std::map<const AtNode*, SdfPath> m_shader_to_usd_path;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
