//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#include "setup.h"

using namespace lark;

Setup::Setup(Navigation *navigation) : View(navigation) {
    Setup::Init();
}

Setup::~Setup() = default;

void Setup::Init() {
    // normal group.
    // row1
    {
        glm::vec3 p(-2.425F, -0.4, 0);
        quick_config_setup_ = std::make_shared<QuickConfigSetup>(SetupGroup_Normal, this);
        quick_config_setup_->Move(p);
        // add to aabb.
        quick_config_setup_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(quick_config_setup_.get());
        AddChild(quick_config_setup_);
        items_.push_back(quick_config_setup_);
    }
    {
        glm::vec3 p(0.075F, -0.4F, 0);
        haptics_feedback_ = std::make_shared<UseHapticsFeedback>(SetupGroup_Normal);
        haptics_feedback_->Move(p);
        haptics_feedback_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(haptics_feedback_.get());
        AddChild(haptics_feedback_);
        items_.push_back(haptics_feedback_);
    }
    // row2
    {
        glm::vec3 p(-2.425F, -0.4 - 1.7F, 0);
        resolution_ = std::make_shared<Resolution>(SetupGroup_Normal, quick_config_setup_.get());
        resolution_->Move(p);
        // add to aabb.
        resolution_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(resolution_.get());
        AddChild(resolution_);
        items_.push_back(resolution_);
    }
    {
        glm::vec3 p(0.075F, -0.4 - 1.7F, 0);
        code_rate_ = std::make_shared<CodeRate>(SetupGroup_Normal, quick_config_setup_.get());
        code_rate_->Move(p);
        // add to aabb
        code_rate_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(code_rate_.get());
        AddChild(code_rate_);
        items_.push_back(code_rate_);
    }
    {
//        glm::vec3 p(0.075F, -0.4F - 1.7F, 0);
//        room_height_ = std::make_shared<RoomHeight>();
//        room_height_->Move(p);
//         add to aabb
//        room_height_->SetAABBPositon(glm::vec2(p.x, p.y));
//        PushAABB(room_height_.get());
//        AddChild(room_height_);
    }
    {
//        glm::vec3 p(0.075F, -0.4F - 1.7F, 0);
//        ipd_ = std::make_shared<Ipd>();
//        ipd_->Move(p);
//         add to aabb
//        ipd_->SetAABBPositon(glm::vec2(p.x, p.y));
//        ipd_->set_active(false);
//        PushAABB(ipd_.get());
//        AddChild(ipd_);
    }
    // advance group
    // row 1
    {
        glm::vec3 p(-2.425F - 0.8, -0.4, 0);
        kcp_setup_ = std::make_shared<KCPSetup>(SetupGroup_Advance, quick_config_setup_.get());
        kcp_setup_->Move(p);
        // add to aabb
        kcp_setup_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(kcp_setup_.get());
        kcp_setup_->set_active(false);
        AddChild(kcp_setup_);
        items_.push_back(kcp_setup_);
    }
    {
        glm::vec3 p(0.075F - 0.8, -0.4, 0);
        h265_setup_ = std::make_shared<H265Setup>(SetupGroup_Advance, quick_config_setup_.get());
        h265_setup_->Move(p);
        // add to aabb
        h265_setup_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(h265_setup_.get());
        h265_setup_->set_active(false);
        AddChild(h265_setup_);
        items_.push_back(h265_setup_);
    }
    {
        glm::vec3 p(2.575F - 0.8, -0.4, 0);
        fec_ = std::make_shared<FECReport>(SetupGroup_Advance, quick_config_setup_.get());
        fec_->Move(p);
        // add to aabb
        fec_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(fec_.get());
        fec_->set_active(false);
        AddChild(fec_);
        items_.push_back(fec_);
    }
    // row2
    {
        glm::vec3 p(-2.425F - 0.8, -0.4F - 1.7F, 0);
        fps_ = std::make_shared<Fps>(SetupGroup_Advance);
        fps_->Move(p);
        // add to aabb.
        fps_->SetAABBPositon(glm::vec2(p.x, p.y));
        fps_->set_active(false);
        PushAABB(fps_.get());
        AddChild(fps_);
        items_.push_back(fps_);
    }
    {
        glm::vec3 p(0.075F - 0.8, -0.4F - 1.7F, 0);
        ffr_setup_ = std::make_shared<FFRSetup>(SetupGroup_Advance, quick_config_setup_.get());
        ffr_setup_->Move(p);
        // add to aabb.
        ffr_setup_->SetAABBPositon(glm::vec2(p.x, p.y));
        ffr_setup_->set_active(false);
        PushAABB(ffr_setup_.get());
        AddChild(ffr_setup_);
        items_.push_back(ffr_setup_);
    }
    {
        glm::vec3 p(2.575F - 0.8, -0.4 - 1.7F, 0);
        use_10bitencoder_ = std::make_shared<Use10BitEncode>(SetupGroup_Advance, quick_config_setup_.get());
        use_10bitencoder_->Move(p);
        // add to aabb
        use_10bitencoder_->SetAABBPositon(glm::vec2(p.x, p.y));
        use_10bitencoder_->set_active(false);
        PushAABB(use_10bitencoder_.get());
        AddChild(use_10bitencoder_);
        items_.push_back(use_10bitencoder_);
    }

    // reset btn.
    {
        glm::vec3 p(-1.425F, 1.5F, 0.01F);
        reset_btn_ = std::make_shared<ResetButton>();
        reset_btn_->Move(p);
        reset_btn_->SetAABBPositon(glm::vec2(p.x, p.y - reset_btn_->GetSize().y / 2.0F));
        PushAABB(reset_btn_.get());
        AddChild(reset_btn_);
    }

    // advance btn
    {
        glm::vec3 p(0, 1.5F, 0.01F);
        advance_btn_ = std::make_shared<TextButton>(L"????????????");
        advance_btn_->Move(p);
        advance_btn_->SetAABBPositon(glm::vec2(p.x, p.y - reset_btn_->GetSize().y / 2.0F));
        PushAABB(advance_btn_.get());
        AddChild(advance_btn_);
    }
    View::Init();
}

void Setup::Update() {
    Object::Update();
    // reset all.
    if (reset_btn_->picked() && Input::IsInputEnter()) {
        for (const auto &i : items_) {
            if (i->group() == group_) {
                i->Reset();
            }
        }
    }
    if (advance_btn_->picked() && Input::IsInputEnter()) {
        if (group_ == SetupGroup_Normal) {
            // switch to advance group.
            group_ = SetupGroup_Advance;
            advance_btn_->SetText(L"????????????");
        } else {
            group_ = SetupGroup_Normal;
            advance_btn_->SetText(L"????????????");
        }

        for(const auto &i : items_) {
            i->set_active(i->group() == group_);
            i->set_aabb_active(i->group() == group_);
        }
    }
}

void Setup::Enter() {
    View::Enter();
    for(const auto& i :items_) {
        i->Enter();
    }
}

void Setup::Leave() {
    View::Leave();
    for(const auto& i :items_) {
        i->Enter();
    }
}

void Setup::FreshData() {
    for(const auto& i :items_) {
        i->FreshData();
    }
}

