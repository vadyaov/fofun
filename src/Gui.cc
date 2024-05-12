#include "Arena.h"

void Arena::ImGuiWindow() {
  ImGui::Begin("FORFUN settings");
  {
    ImGui::ColorEdit4("Background color", (float*)&backColor);
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Circles: %ld", circles.size()); ImGui::SameLine();
    ImGui::Text("Walls: %ld", walls.size()); ImGui::SameLine();
    ImGui::Text("Total Energy: %f", totalEnergy);
    totalEnergy = 0.0f;ImGui::SameLine();
    if (ImGui::Button("Clear")) {
      clearArena();
    } ImGui::Separator();

    ImGui::Text("WALLS");
    {
      ImGui::Checkbox("top", &borders[0].first); ImGui::SameLine();
      ImGui::Checkbox("bot", &borders[1].first); ImGui::SameLine();
      ImGui::Checkbox("left", &borders[2].first); ImGui::SameLine();
      ImGui::Checkbox("right", &borders[3].first); // ImGui::SameLine();


      /* for (int i = 0; i != 4; ++i) { */
      /*   if (borders[i].first == true && !borders[i].second) { */
      /*     borders[i].second = std::make_shared<Wall>(brdrs[i]); */
      /*   } else if (borders[i].first == false && borders[i].second) { */
      /*     borders[i].second.reset(); */
      /*   } */
      /* } */

    } ImGui::Separator();
   
    ImGui::Text("CREATION");
    {
      if (ImGui::RadioButton("Ball", shapeType == Circle)) { shapeType = Circle; } ImGui::SameLine();
      if (ImGui::RadioButton("Wall", shapeType == Rectangle)) { shapeType = Rectangle; } ImGui::SameLine();
      if (ImGui::RadioButton("Triangle", shapeType == Triangle)) { shapeType = Triangle; }

      ImGui::ColorEdit4("color ", (float*)&creationParameters.shapeColor); ImGui::SameLine();
      ImGui::Checkbox("random##1", &creationParameters.randomColor);
      if (shapeType == Circle) {
        ImGui::SliderFloat("radius", &creationParameters.radius, 10.0f, 20.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##2", &creationParameters.randomRadius);
        ImGui::SliderFloat("speed ", &creationParameters.speed, 0.0f, 200.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##3", &creationParameters.randomSpeed);
        ImGui::SliderInt("angle ", &creationParameters.angle, 0, 360); ImGui::SameLine();
        ImGui::Checkbox("random##4", &creationParameters.randomAngle);
      } else if (shapeType == Rectangle) {
        ImGui::SliderFloat("width", &creationParameters.radius, 5.0f, 20.0f, "%.1f");
      }

    } ImGui::Separator();

    ImGui::Text("GRAVITY");
    {
      if (ImGui::RadioButton("None", gravityType == NoGravity)) {
        gravityType = NoGravity;
        switchGravity(gravityType);
      } ImGui::SameLine();
      if (ImGui::RadioButton("Earth", gravityType == Earth)) {
        gravityType = Earth;
        switchGravity(gravityType);
      }
    }
  } ImGui::End();
}
