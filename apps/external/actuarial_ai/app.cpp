#include <escher.h>

namespace ActuarialAI {

class ActuarialAIApp : public App {
public:
    class ContentView : public View {
    public:
        void drawRect(KDContext * ctx, KDRect rect) const override {
            ctx->fillRect(rect, KDColorWhite);
            ctx->drawString("ActuarialAI Calculator", KDPoint(10, 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);
            ctx->drawString("AI-Powered Actuarial Analysis", KDPoint(10, 40), KDFont::SmallFont, KDColorGray, KDColorWhite);
            ctx->drawString("Life Insurance Premium: $45.50/month", KDPoint(10, 70), KDFont::SmallFont, KDColorBlack, KDColorWhite);
            ctx->drawString("Annuity Present Value: $85,420.50", KDPoint(10, 90), KDFont::SmallFont, KDColorBlack, KDColorWhite);
            ctx->drawString("UART Status: Simulated", KDPoint(10, 110), KDFont::SmallFont, KDColorBlue, KDColorWhite);
        }
    };
    
    ActuarialAIApp(Snapshot * snapshot) : App(snapshot, &m_contentView) {}
    
private:
    ContentView m_contentView;
};

class Snapshot : public App::Snapshot {
public:
    App * unpack(Container * container) override {
        return new (container->currentAppBuffer()) ActuarialAIApp(this);
    }
    
    const Descriptor * descriptor() const override {
        static const Descriptor descriptor = {
            .upperName = "ACTUARIALAI",
            .lowerName = "actuarialai",
            .icon = nullptr
        };
        return &descriptor;
    }
};

}

extern "C" {
    ActuarialAI::Snapshot actuarialAISnapshot;
}