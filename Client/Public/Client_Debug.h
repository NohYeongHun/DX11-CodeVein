#pragma once



#ifdef _DEBUG
static void Edit_OBB(CCollider* pColliderCom)
{
    CBounding_OBB::BOUNDING_OBB_DESC* pDesc =
        static_cast<CBounding_OBB::BOUNDING_OBB_DESC*>(pColliderCom->Get_BoundingDesc());

    static _float vCenter[3] = { pDesc->vCenter.x,  pDesc->vCenter.y, pDesc->vCenter.z };
    static _float vRotation[3] = { pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z };
    static _float vExtents[3] = { pDesc->vExtents.x, pDesc->vExtents.y, pDesc->vExtents.z };

    /* 정보 표시. */
    ImGui::Text("Center : (%.2f, %.2f, %.2f)", vCenter[0], vCenter[1], vCenter[2]);
    ImGui::Separator();

    ImGui::Text("Rotation : (%.2f, %.2f, %.2f)", vRotation[0], vRotation[1], vRotation[2]);
    ImGui::Separator();

    ImGui::Text("Extents : (%.2f, %.2f, %.2f)", vExtents[0], vExtents[1], vExtents[2]);
    ImGui::Separator();

    ImGui::InputFloat3("Center", vCenter);
    ImGui::InputFloat3("Rotation", vRotation);
    ImGui::InputFloat3("Scale", vExtents);

    if (ImGui::Button("Apply"))
    {
        CBounding_OBB::BOUNDING_OBB_DESC Desc{};
        Desc = *pDesc;
        memcpy(&Desc.vCenter, vCenter, sizeof(_float3));
        memcpy(&Desc.vRotation, vRotation, sizeof(_float3));
        memcpy(&Desc.vExtents, vExtents, sizeof(_float3));
        pColliderCom->Change_BoundingDesc(&Desc);
    }
}


static void Edit_AABB(CCollider* pColliderCom)
{
    CBounding_AABB::BOUNDING_AABB_DESC* pDesc =
        static_cast<CBounding_AABB::BOUNDING_AABB_DESC*>(pColliderCom->Get_BoundingDesc());

    static _float vCenter[3] = { pDesc->vCenter.x,  pDesc->vCenter.y, pDesc->vCenter.z };
    static _float vExtents[3] = { pDesc->vExtents.x, pDesc->vExtents.y, pDesc->vExtents.z };

    /* 정보 표시. */
    ImGui::Text("Center : (%.2f, %.2f, %.2f)", vCenter[0], vCenter[1], vCenter[2]);
    ImGui::Separator();

    ImGui::Text("Extents : (%.2f, %.2f, %.2f)", vExtents[0], vExtents[1], vExtents[2]);
    ImGui::Separator();

    ImGui::InputFloat3("Center", vCenter);
    ImGui::InputFloat3("Scale", vExtents);

    if (ImGui::Button("Apply"))
    {
        CBounding_AABB::BOUNDING_AABB_DESC Desc{};
        Desc = *pDesc;
        memcpy(&Desc.vCenter, vCenter, sizeof(_float3));
        memcpy(&Desc.vExtents, vExtents, sizeof(_float3));
        pColliderCom->Change_BoundingDesc(&Desc);
    }
}

static void Edit_SPEHRE(CCollider* pColliderCom)
{
    CBounding_Sphere::BOUNDING_SPHERE_DESC* pDesc = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(pColliderCom->Get_BoundingDesc());

    static _float vCenter[3] = { pDesc->vCenter.x,  pDesc->vCenter.y, pDesc->vCenter.z };
    static _float fRadius = pDesc->fRadius;

    /* 정보 표시. */
    ImGui::Text("Center : (%.2f, %.2f, %.2f)", vCenter[0], vCenter[1], vCenter[2]);
    ImGui::Separator();

    ImGui::Text("Radius : (%.2f)", fRadius);
    ImGui::Separator();

    ImGui::InputFloat3("Center", vCenter);
    ImGui::InputFloat("Radius", &fRadius);

    if (ImGui::Button("Apply"))
    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC Desc{};
        Desc = *pDesc;
        memcpy(&Desc.vCenter, vCenter, sizeof(_float3));
        memcpy(&Desc.fRadius, &fRadius, sizeof(_float));
        pColliderCom->Change_BoundingDesc(&Desc);
    }
}

static void Edit_Collider(CCollider* pColliderCom, string strTag)
{

    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window

    ImVec2 windowSize = ImVec2(300.f, 300.f);
    ImVec2 windowPos = ImVec2(io.DisplaySize.x - windowSize.x, windowSize.y);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = strTag + " Debug";
    ImGui::Begin( strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    switch(pColliderCom->Get_ColliderShape())
    {
        case COLLIDER::OBB:
            Edit_OBB(pColliderCom);
            break;
        case COLLIDER::AABB:
            Edit_AABB(pColliderCom);
            break;
        case COLLIDER::SPHERE:
            Edit_SPEHRE(pColliderCom);
            break;
        default:
            break;
    }
    ImGui::End();
}


#endif // _DEBUG

