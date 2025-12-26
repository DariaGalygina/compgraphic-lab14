#include "main.h"

void Init()
{
	pl.pos = glm::vec3(-3.12f, 8.27f, -2.83f);
	pl.ambient = glm::vec3(0.1f);
	pl.diffuse = glm::vec3(1.0f);
	pl.specular = glm::vec3(1.0f);
	pl.atten = glm::vec3(0.2f);

	dl.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	dl.ambient = glm::vec3(0.25f);
	dl.diffuse = glm::vec3(0.25f);
	dl.specular = glm::vec3(0.25f);

	sl.pos = glm::vec3(-8.0f, -9.37f, -8.0f);
	sl.direction = glm::vec3(1.0f);
	sl.ambient = glm::vec3(1.0f);
	sl.diffuse = glm::vec3(1.0f);
	sl.specular = glm::vec3(1.0f);
	sl.cutoff = 1.5f;
	sl.atten = glm::vec3(0.1f, 0.1f, 0.1f);

	mat.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
	mat.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	mat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	mat.emission = glm::vec3(0.0f, 0.0f, 0.0f);
	mat.shininess = 1.0f;

	glEnable(GL_DEPTH_TEST);
	InitShader();
	InitVBO();
	InitTextures();
}

int main()
{
	setlocale(LC_ALL, "Russian");

	sf::Window window(sf::VideoMode(sf::Vector2u(1000, 1000)), "Lab14");

	SetIcon(window);
	window.setVerticalSyncEnabled(true); 
	window.setActive(true); 
	glewInit();
	Init(); 

	bool paused_sun = false;
	bool paused_axis = false;

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) 
			{
				window.close();
				goto EXIT_IS_RIGHT_HERE; 
			}
			else if (const sf::Event::Resized* resized = event->getIf<sf::Event::Resized>()) 
			{
				glViewport(0, 0, resized->size.x, resized->size.y); 
			}
			else if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
			{
				if (keyPressed->code == sf::Keyboard::Key::Up)
				{
					cam.PitchPlus();
				}
				if (keyPressed->code == sf::Keyboard::Key::Down)
				{
					cam.PitchMinus();
				}
				if (keyPressed->code == sf::Keyboard::Key::Right)
				{
					cam.YawPlus();
				}
				if (keyPressed->code == sf::Keyboard::Key::Left)
				{
					cam.YawMinus();
				}

				if (keyPressed->code == sf::Keyboard::Key::W)
				{
					cam.W();
				}
				if (keyPressed->code == sf::Keyboard::Key::S)
				{
					cam.S();
				}
				if (keyPressed->code == sf::Keyboard::Key::A)
				{
					cam.A();
				}
				if (keyPressed->code == sf::Keyboard::Key::D)
				{
					cam.D();
				}

				if (keyPressed->code == sf::Keyboard::Key::F1)
				{
					cam.Perspective();
				}

				if (keyPressed->code == sf::Keyboard::Key::F2)
				{
					cam.Ortho();
				}

				if (keyPressed->code == sf::Keyboard::Key::Escape)
				{
					cam.Reset();
				}

				if (keyPressed->code == sf::Keyboard::Key::Num1)
				{
					pl.Config();
				}
				if (keyPressed->code == sf::Keyboard::Key::Num2)
				{
					dl.Config();
				}
				if (keyPressed->code == sf::Keyboard::Key::Num3)
				{
					sl.Config();
				}
				if (keyPressed->code == sf::Keyboard::Key::F3)  
				{
					currentLightingModel = LightingModel::PHONG;
					std::cout << "Switched to Phong lighting model for ALL objects" << std::endl;
				}
				else if (keyPressed->code == sf::Keyboard::Key::F4)
				{
					currentLightingModel = LightingModel::TOON;
					std::cout << "Switched to Toon lighting model for ALL objects" << std::endl;
				}
				else if (keyPressed->code == sf::Keyboard::Key::F5) 
				{
					currentLightingModel = LightingModel::BIDIR;
					std::cout << "Switched to Bidirectional lighting model for ALL objects" << std::endl;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		Draw(window); 
		window.display(); 
	}

EXIT_IS_RIGHT_HERE: 
	Release(); 
	return 0; 
}


void LoadObject(int i, const char* path)
{
	glGenBuffers(1, &Objects[i]); 
	ifstream testFile(path);
	if (!testFile.good()) {
		cout << "ERROR: File not found: " << path << endl;
		return;
	}
	testFile.close();


	cout << "Loading object " << i << " from: " << path << endl;

	vector<Vertex> data;
	VERTICES[i] = load_obj(path, data);

	if (VERTICES[i] <= 0) {
		cout << "ERROR: Failed to load object or object has 0 vertices!" << endl;
		return;
	}

	cout << "Loaded " << VERTICES[i] << " vertices" << endl;

	glGenBuffers(1, &Objects[i]);
	glBindBuffer(GL_ARRAY_BUFFER, Objects[i]);
	glBufferData(GL_ARRAY_BUFFER, VERTICES[i] * sizeof(Vertex), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkOpenGLerror();
}

void InitVBO()
{
	LoadObject(0, "models/plane.obj");
	LoadObject(1, "models/cube.obj");
	LoadObject(2, "models/torus.obj");
	LoadObject(3, "models/suzanne.obj");
	LoadObject(4, "models/cylinder.obj");
	LoadObject(5, "models/cone.obj");
}

void InitTextures()
{
	LoadTexture(GL_TEXTURE0, textures[0], "textures/snow.png");
	LoadTexture(GL_TEXTURE1, textures[1], "textures/my_pig.jpg");
	LoadTexture(GL_TEXTURE2, textures[2], "textures/ikra.png");
	LoadTexture(GL_TEXTURE3, textures[3], "textures/sky.png");
	LoadTexture(GL_TEXTURE4, textures[4], "textures/pryan.png");
	LoadTexture(GL_TEXTURE5, textures[5], "textures/elka.png");
}

void LoadAttrib(GLuint prog, GLint& attrib, const char* attr_name)
{
	attrib = glGetAttribLocation(prog, attr_name);
	if (attrib == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}
}

void LoadUniform(GLuint prog, GLint& attrib, const char* attr_name)
{
	attrib = glGetUniformLocation(prog, attr_name);
	if (attrib == -1)
	{
		std::cout << "could not bind uniform " << attr_name << std::endl;
		return;
	}
}

void LoadTexture(GLenum tex_enum, GLuint& tex, const char* path)
{
	glGenTextures(1, &tex); 
	glActiveTexture(tex_enum);
	glBindTexture(GL_TEXTURE_2D, tex); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	sf::Image img;
	if (!img.loadFromFile(path))
	{
		std::cout << "could not load texture " << path << std::endl;
		return;
	}

	sf::Vector2u size = img.getSize();
	int width = size.x;
	int height = size.y;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void InitShader()
{
	GLuint PhongVShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(PhongVShader, 1, load_shader("shaders/phong.vert"), NULL);
	glCompileShader(PhongVShader);
	std::cout << "phong vertex shader \n";
	ShaderLog(PhongVShader);

	GLuint PhongFShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(PhongFShader, 1, load_shader("shaders/phong.frag"), NULL);
	glCompileShader(PhongFShader);
	std::cout << "phong fragment shader \n";
	ShaderLog(PhongFShader);

	GLuint ToonVShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ToonVShader, 1, load_shader("shaders/toon.vert"), NULL);
	glCompileShader(ToonVShader);
	std::cout << "toon vertex shader \n";
	ShaderLog(ToonVShader);

	GLuint ToonFShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ToonFShader, 1, load_shader("shaders/toon.frag"), NULL);
	glCompileShader(ToonFShader);
	std::cout << "toon fragment shader \n";
	ShaderLog(ToonFShader);

	GLuint BidirVShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(BidirVShader, 1, load_shader("shaders/bidir.vert"), NULL);
	glCompileShader(BidirVShader);
	std::cout << "bidir vertex shader \n";
	ShaderLog(BidirVShader);

	GLuint BidirFShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(BidirFShader, 1, load_shader("shaders/bidir.frag"), NULL);
	glCompileShader(BidirFShader);
	std::cout << "bidir fragment shader \n";
	ShaderLog(BidirFShader);

	Programs[0] = glCreateProgram(); // Phong
	Programs[1] = glCreateProgram(); // Toon
	Programs[2] = glCreateProgram(); // Bidir

	glAttachShader(Programs[0], PhongVShader);
	glAttachShader(Programs[0], PhongFShader);

	glAttachShader(Programs[1], ToonVShader);
	glAttachShader(Programs[1], ToonFShader);

	glAttachShader(Programs[2], BidirVShader);
	glAttachShader(Programs[2], BidirFShader);

	glLinkProgram(Programs[0]);
	glLinkProgram(Programs[1]);
	glLinkProgram(Programs[2]);

	int link1, link2, link3;
	glGetProgramiv(Programs[0], GL_LINK_STATUS, &link1);
	glGetProgramiv(Programs[1], GL_LINK_STATUS, &link2);
	glGetProgramiv(Programs[2], GL_LINK_STATUS, &link3);

	if (!link1 || !link2 || !link3)
	{
		std::cout << "could not link shader program" << std::endl;
		return;
	}

	LoadAttrib(Programs[0], Phong_coord, "coord");
	LoadAttrib(Programs[0], Phong_texcoord, "texcoord");
	LoadAttrib(Programs[0], Phong_normal, "normal");
	LoadUniform(Programs[0], Phong_mvp, "mvp");
	LoadUniform(Programs[0], Phong_viewPos, "viewPos");

	LoadAttrib(Programs[1], Toon_coord, "coord");
	LoadAttrib(Programs[1], Toon_texcoord, "texcoord");
	LoadAttrib(Programs[1], Toon_normal, "normal");
	LoadUniform(Programs[1], Toon_mvp, "mvp");
	LoadUniform(Programs[1], Toon_viewPos, "viewPos");

	LoadAttrib(Programs[2], Bidir_coord, "coord");
	LoadAttrib(Programs[2], Bidir_texcoord, "texcoord");
	LoadAttrib(Programs[2], Bidir_normal, "normal");
	LoadUniform(Programs[2], Bidir_mvp, "mvp");
	checkOpenGLerror();
}

LightingModel currentLightingModel = LightingModel::PHONG;

void SetupAttributes(LightingModel model, GLuint vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	switch (model) {
	case LightingModel::PHONG:
		glVertexAttribPointer(Phong_coord, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(Phong_texcoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(Phong_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
		break;
	case LightingModel::TOON:
		glVertexAttribPointer(Toon_coord, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(Toon_texcoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(Toon_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
		break;
	case LightingModel::BIDIR:
		glVertexAttribPointer(Bidir_coord, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(Bidir_texcoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(Bidir_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
		break;
	}
}

void EnableAttributes(LightingModel model)
{
	switch (model) {
	case LightingModel::PHONG:
		glEnableVertexAttribArray(Phong_coord);
		glEnableVertexAttribArray(Phong_texcoord);
		glEnableVertexAttribArray(Phong_normal);
		break;
	case LightingModel::TOON:
		glEnableVertexAttribArray(Toon_coord);
		glEnableVertexAttribArray(Toon_texcoord);
		glEnableVertexAttribArray(Toon_normal);
		break;
	case LightingModel::BIDIR:
		glEnableVertexAttribArray(Bidir_coord);
		glEnableVertexAttribArray(Bidir_texcoord);
		glEnableVertexAttribArray(Bidir_normal);
		break;
	}
}

void DisableAttributes(LightingModel model)
{
	switch (model) {
	case LightingModel::PHONG:
		glDisableVertexAttribArray(Phong_coord);
		glDisableVertexAttribArray(Phong_texcoord);
		glDisableVertexAttribArray(Phong_normal);
		break;
	case LightingModel::TOON:
		glDisableVertexAttribArray(Toon_coord);
		glDisableVertexAttribArray(Toon_texcoord);
		glDisableVertexAttribArray(Toon_normal);
		break;
	case LightingModel::BIDIR:
		glDisableVertexAttribArray(Bidir_coord);
		glDisableVertexAttribArray(Bidir_texcoord);
		glDisableVertexAttribArray(Bidir_normal);
		break;
	}
}


void Draw(sf::Window& window)
{
	GLuint tex_loc;
	GLuint programToUse;

	switch (currentLightingModel) {
	case LightingModel::PHONG:
		programToUse = Programs[0];
		break;
	case LightingModel::TOON:
		programToUse = Programs[1];
		break;
	case LightingModel::BIDIR:
		programToUse = Programs[2];
		break;
	default:
		programToUse = Programs[0];
	}

	glUseProgram(programToUse);

	tex_loc = glGetUniformLocation(programToUse, "tex");
	pl.Load(programToUse);
	dl.Load(programToUse);
	sl.Load(programToUse);
	mat.Load(programToUse);

	glm::mat4 view = cam.View();
	glm::mat4 projection = cam.Proj();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
	glm::mat4 mvp = projection * view * model;
	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform3fv(Phong_viewPos, 1, glm::value_ptr(cam.Pos));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform3fv(Toon_viewPos, 1, glm::value_ptr(cam.Pos));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	EnableAttributes(currentLightingModel);

	glUniform1i(tex_loc, 0);
	SetupAttributes(currentLightingModel, Objects[0]);
	glDrawArrays(GL_TRIANGLES, 0, VERTICES[0]);

	glUniform1i(tex_loc, 1);
	SetupAttributes(currentLightingModel, Objects[1]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); 
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	mvp = projection * view * model;

	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	glDrawArrays(GL_TRIANGLES, 0, VERTICES[1]);

	glUniform1i(tex_loc, 2);
	SetupAttributes(currentLightingModel, Objects[2]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -1.0f, 0.0f)); 
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	mvp = projection * view * model;

	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	glDrawArrays(GL_TRIANGLES, 0, VERTICES[2]);

	glUniform1i(tex_loc, 3);
	SetupAttributes(currentLightingModel, Objects[3]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, -1.0f, 0.0f)); 
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	mvp = projection * view * model;

	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	glDrawArrays(GL_TRIANGLES, 0, VERTICES[3]);

	glUniform1i(tex_loc, 4);
	SetupAttributes(currentLightingModel, Objects[4]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 4.0f)); 
	model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
	mvp = projection * view * model;

	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	glDrawArrays(GL_TRIANGLES, 0, VERTICES[4]);

	glUniform1i(tex_loc, 5);
	SetupAttributes(currentLightingModel, Objects[5]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, -4.0f)); 
	model = glm::scale(model, glm::vec3(1.2f, 2.0f, 1.2f));
	mvp = projection * view * model;

	if (currentLightingModel == LightingModel::PHONG) {
		glUniformMatrix4fv(Phong_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::TOON) {
		glUniformMatrix4fv(Toon_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	else if (currentLightingModel == LightingModel::BIDIR) {
		glUniformMatrix4fv(Bidir_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	glDrawArrays(GL_TRIANGLES, 0, VERTICES[5]);

	DisableAttributes(currentLightingModel);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	checkOpenGLerror();
}

void Release()
{
	ReleaseShader(); 
	ReleaseVBO();
}

void ReleaseVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	for (int i = 0; i < Objects.size(); i++)
	{
		glDeleteBuffers(1, &Objects[i]); 
	}
}

void ReleaseShader()
{
	glUseProgram(0); 
	for (int i = 0; i < Programs.size(); i++)
	{
		glDeleteProgram(Programs[i]); 
	}
}

void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
		exit(1);
	}
}

void checkOpenGLerror()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error: " << err << std::endl;
	}
}

void SetIcon(sf::Window& wnd)
{
	sf::Image image;
	if (!image.loadFromFile("icon.png"))
	{
		std::cout << "error load icon \n";
		return;
	}

	wnd.setIcon(image.getSize(), image.getPixelsPtr());
}
