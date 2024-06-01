#include "Arena.h"

void Arena::ImguiWindow() {
  ImGui::Begin("FORFUN settings");
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Balls: %ld", balls.size()); ImGui::SameLine();
    ImGui::Text("Walls: %ld", walls.size()); ImGui::SameLine();
    ImGui::Text("Energy: %f", totalEnergy);
    totalEnergy = 0.0f; ImGui::SameLine();

    ImGui::Separator();

    ImGui::ColorEdit4("Background color", (float*)&backColor);

    ImGui::Text("WALL");
    {
      ImGui::Checkbox("top", &borders[0].first); ImGui::SameLine();
      ImGui::Checkbox("bot", &borders[1].first); ImGui::SameLine();
      ImGui::Checkbox("left", &borders[2].first); ImGui::SameLine();
      ImGui::Checkbox("right", &borders[3].first);

    } ImGui::Separator();
   
    ImGui::Text("CREATE");
    {
      if (ImGui::RadioButton("Ball", shapeType == SPHERE)) { shapeType = SPHERE; } ImGui::SameLine();
      if (ImGui::RadioButton("Wall", shapeType == WALL)) { shapeType = WALL; } ImGui::SameLine();
      if (ImGui::RadioButton("Source", shapeType == SOURCE)) { shapeType = SOURCE; }

      ImGui::ColorEdit4("color ", (float*)&creationParameters.shapeColor); ImGui::SameLine();
      ImGui::Checkbox("random##1", &creationParameters.randomColor);
      if (shapeType == SPHERE || shapeType == SOURCE) {
        ImGui::SliderFloat("radius", &creationParameters.radius, 10.0f, 20.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##2", &creationParameters.randomRadius);
        ImGui::SliderFloat("speed ", &creationParameters.speed, 0.0f, 200.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##3", &creationParameters.randomSpeed);
        ImGui::SliderInt("angle ", &creationParameters.angle, 0, 360); ImGui::SameLine();
        ImGui::Checkbox("random##4", &creationParameters.randomAngle);

        if (shapeType == SOURCE) {
          ImGui::SliderFloat("spawn period", &creationParameters.spawnPeriod, 0.1f, 5.0f, "%.01f");
        }
      } else if (shapeType == WALL) {
        ImGui::SliderFloat("width", &creationParameters.radius, 5.0f, 20.0f, "%.1f");
      }
    } ImGui::Separator();

    ImGui::Text("DESTROY");
    {

      if (ImGui::Button("Balls")) {
        balls.clear();
      } ImGui::SameLine();

      if (ImGui::Button("Walls")) {
        walls.clear();
      } ImGui::SameLine();

      if (ImGui::Button("ALL")) {
        balls.clear();
        walls.clear();
      } // ImGui::SameLine();

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
    } ImGui::Separator();

    ImGui::Text("MUSIC");
    {
      if (ImGui::TreeNode("Sounds")) {
          static int selected = -1;
          auto s = sounds->getSounds();
          for (int n = 0; n < s.size(); n++)
          {
              char buf[100];
              sprintf(buf, "%s", s[n].filename().c_str());
              if (ImGui::Selectable(buf, selected == n)) {
                  selected = n;
                if (!music.openFromFile(s[n].string()))
                {
                  std::cout << "error\n";
                }
                music.setPitch(1);           // increase the pitch
                music.setVolume(100);         // reduce the volume
                music.setLoop(false);         // make it loop
                 
                // Play it
                music.play();
              }
          }
          ImGui::TreePop();
      }
    } ImGui::Separator();

    if (ImGui::RadioButton("Show Collisions", showCollisions == true)) {showCollisions = !showCollisions;}
    if (ImGui::RadioButton("PAUSE", pause == true)) { pause = !pause; }

  } ImGui::End();
}
