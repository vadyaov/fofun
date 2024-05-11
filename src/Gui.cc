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
      ImGui::Checkbox("top", &bool_borders[0]); ImGui::SameLine();
      ImGui::Checkbox("bot", &bool_borders[1]); ImGui::SameLine();
      ImGui::Checkbox("left", &bool_borders[2]); ImGui::SameLine();
      ImGui::Checkbox("right", &bool_borders[3]); // ImGui::SameLine();

      if (bool_borders[0] == true && (!mp.count(0) || mp[0] == borders.end())) {
        mp[0] = borders.insert(borders.cend(), std::make_shared<Wall>(TopBorder(size)));

        bool_borders[1] = true;
      } else if (bool_borders[0] == false && mp.count(0) == 1 && mp[0] != borders.end()) {
        borders.erase(mp[0]);
        mp[0] = borders.end();

        bool_borders[1] = false;
      }

      if (bool_borders[1] == true && (!mp.count(1) || mp[1] == borders.end())) {
        mp[1] = borders.insert(borders.cend(), std::make_shared<Wall>(BotBorder(size)));

        bool_borders[0] = true;
      } else if (bool_borders[1] == false && mp.count(1) == 1 && mp[1] != borders.end()) {
        borders.erase(mp[1]);
        mp[1] = borders.end();

        bool_borders[0] = false;
      }

      if (bool_borders[2] == true && (!mp.count(2) || mp[2] == borders.end())) {
        mp[2] = borders.insert(borders.cend(), std::make_shared<Wall>(LeftBorder(size)));

        bool_borders[3] = true;
      } else if (bool_borders[2] == false && mp.count(2) == 1 && mp[2] != borders.end()) {
        borders.erase(mp[2]);
        mp[2] = borders.end();

        bool_borders[3] = false;
      }

      if (bool_borders[3] == true && (!mp.count(3) || mp[3] == borders.end())) {
        mp[3] = borders.insert(borders.cend(), std::make_shared<Wall>(RightBorder(size)));

        bool_borders[2] = true;
      } else if (bool_borders[3] == false && mp.count(3) == 1 && mp[3] != borders.end()) {
        borders.erase(mp[3]);
        mp[3] = borders.end();

        bool_borders[2] = false;
      }

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
