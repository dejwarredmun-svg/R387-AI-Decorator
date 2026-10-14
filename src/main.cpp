
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(R387EditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto sprite = CCLabelBMFont::create("R387 AI", "bigFont.fnt");
        if (!sprite) {
            return true;
        }

        sprite->setScale(0.55f);

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(R387EditorUI::onR387Decorator)
        );

        if (!button) {
            return true;
        }

        button->setID("r387-ai-decorator-button");

        auto menu = CCMenu::create();
        if (!menu) {
            return true;
        }

        menu->addChild(button);
        menu->setID("r387-ai-decorator-menu");
        menu->setPosition({30.f, 120.f});

        this->addChild(menu, 1000);

        return true;
    }

    void onR387Decorator(CCObject*) {
        FLAlertLayer::create(
            "R387 AI Decorator",
            "The R387 editor button works!\n\n"
            "Automatic object placement is not implemented yet.",
            "OK"
        )->show();
    }
};
