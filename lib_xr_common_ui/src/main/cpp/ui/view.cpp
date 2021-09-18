//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include "view.h"
#include "ui/aa_bb.h"
#include "navigation.h"

#define LOG_TAG "view"

namespace {
    const char * DOT             = "textures/ui/circle_active.png";
    const char * ACTIVIVE        = "textures/ui/circle.png";
}

using namespace lark;

float View::VIEW_POSITION_X = 0.0f;
float View::VIEW_POSITION_Y = 0.0F;
float View::VIEW_POSITION_Z = -4.0F;
float View::VIEW_WIDTH      = 6.0F;
float View::VIEW_HEIGHT     = 4.0F;

View::View(Navigation *navigation):
    navigation_(navigation),
    plane_(),
    aabb_list_(),
    ray_point_(),
    back_btn_(new BackButton),
    bg_(new ColorBox)
{
    bg_->set_color(component::color(0, 0, 0.1F, 0.15F));
    bg_->set_size(component::size(VIEW_WIDTH, VIEW_HEIGHT));
    bg_->Move(-VIEW_WIDTH / 2, -VIEW_HEIGHT / 2 - 0.5F, - 0.1F);
//    AddChild(bg_);

    plane_ = {
            glm::vec3(0, 0, 1), // normal
            glm::vec3(VIEW_POSITION_X, VIEW_POSITION_Y, VIEW_POSITION_Z + 0.2F), // dot
    };
}

View::~View() {
    // clear aabb.
    ClearAABB();
};

void View::Init() {
    if (back_btn_ && back_btn_->active()) {
        back_btn_->Move(-2.425F, 1.5F, 0.01F);
        // add to aabb
        back_btn_->SetAABBPositon(glm::vec2(-2.42F, 1.5F - back_btn_->GetSize().y / 2.0F));
        PushAABB(back_btn_.get());
        AddChild(back_btn_);
    }

    for (auto & rayDot : ray_dots_) {
        rayDot = std::make_shared<Image>();
        rayDot->SetPath(DOT, true);
        rayDot->set_size(glm::vec2(0.04F, 0.04F));
        // Init far away.
        rayDot->Move(100, 100, 100);
        AddChild(rayDot);
    }
}

void View::HandleInput(Ray * rays, int rayCount) {
    // TODO setup ray.

    if (Input::GetCurrentRayCastType() != main_ray_cast_) {
        main_ray_cast_ = Input::GetCurrentRayCastType();
        ray_dots_[main_ray_cast_]->SetPath(ACTIVIVE, true);
        for (int i = 0; i < Input::RayCast_Count; i++) {
            if (main_ray_cast_ != i) {
                ray_dots_[i]->SetPath(DOT, true);
            }
        }
    }

    //    float t;
    //    float dot = utils::Dot(foward, normal);
    //    glm::vec3 f = planeDot - ray.p;
    //    float t =  utils::Dot(f, normal) / dot;
    //    0 -> left 1 -> right for now.
    Plane plane = plane_;
    for (int i = 0; i < rayCount; i ++) {
        Ray ray = rays[i];
        float t = (plane.dot.z - ray.ori.z) / ray.dir.z;
        if (t > 0) {
            continue;
        }

        Transform parentTransforms = GetTransforms();
        glm::vec3 parentPosition = parentTransforms.GetPosition();

        glm::vec3 p;
        p.x = ray.ori.x + t * ray.dir.x - parentPosition.x;
        p.y = ray.ori.y + t * ray.dir.y - parentPosition.y;
        p.z = ray.ori.z + t * ray.dir.z - parentPosition.z;
        Transform transform;
        transform.Translate(p);

        ray_dots_[i]->set_transform(transform);


        //
        ray_point_[i].x = p.x;
        ray_point_[i].y = p.y;
    }
    for(auto& aabb : aabb_list_) {
        if (aabb->aabb_active()) {
            aabb->HandleInput(ray_point_, rayCount);
        }
    }

    if (navigation_ != nullptr &&
        back_btn_ && back_btn_->active() && back_btn_->picked() &&
            (Input::GetCurrentInputState().triggerShortPressed || Input::GetCurrentInputState().enterShortPressed)) {
        // back to home
        navigation_->SetRouter(Navigation::HOME);
    }
}


void View::Enter() {
}

void View::Leave() {
}

void View::PushAABB(AABB * aabb) {
    if (aabb != nullptr)
        aabb_list_.push_back(aabb);
}

void View::ClearAABB() {
    aabb_list_.clear();
}
