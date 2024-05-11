# include <Siv3D.hpp> // Siv3D v0.6.14

namespace constants {
	/// @brief ボール用定数
	namespace ball {
		/// @brief 半径
		constexpr int RAD = 40;
		/// @brief スピード
		constexpr float SPEED = 250.0;
	}

	namespace brick {
		constexpr Size SIZE{ 40, 20 };

		constexpr int X_COUNT = 20;
		constexpr int Y_COUNT = 5;
		constexpr int MAX = X_COUNT * Y_COUNT;
	}

	namespace paddle {
		constexpr Size SIZE{ 60, 10 };
	}
}

class Ball {
public:
	/// @brief ボールの速度
	Vec2 velocity;
	/// @brief ボール
	Circle ball;

	Ball() : velocity({ 0, -constants::ball::SPEED }), ball({ 400, 400, 8 }) {}

	void Draw() const {
		ball.draw();
	}

	void Update() {
		ball.moveBy(velocity * Scene::DeltaTime());
	}
};

class Bricks {
public:
	Rect brickTable[ constants::brick::MAX ];

	Bricks(){
		using namespace::constants::brick;

		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x)
			{
				int index = (y * X_COUNT) + x;

				brickTable[index] = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE
				};
			}
		}
	}

	void Draw() {
		using namespace::constants::brick;

		for (int i = 0; i < MAX; ++i) {
			brickTable[i].stretched(-1).draw(HSV{ brickTable[i].y - 40 });
		}
	}

	void Intersects(Ball* target) {
		using namespace constants::brick;

		// ブロックとの衝突を検知
		for (int i = 0; i < MAX; ++i) {
			auto& refBrick = brickTable[i];

			if (refBrick.intersects(target->ball)) {

				// ブロックの上辺、または底辺と交差
				if (refBrick.bottom().intersects(target->ball) || refBrick.top().intersects(target->ball)) {
					target->velocity.y *= -1;
				}
				else { // ブロックとの左辺、または右辺と交差
					target->velocity.x *= -1;
				}

				// あたったブロックは画面外に出す
				refBrick.y -= 600;

				// 同一フレームでは複数のブロック衝突を検知しない
				break;
			}
		}

		// 天井との衝突
		if ((target->ball.y < 0) && (target->velocity.y < 0)) {
			target->velocity.y *= -1;
		}

		//　壁との衝突
		if (((target->ball.x < 0) && (target->velocity.x < 0)) || ((Scene::Width() < target->ball.x) && (0 < target->velocity.x))) {
			target->velocity.x *= -1;
		}
	}
};

class Paddle {
public:
	Rect paddle;

	Paddle() : paddle({ Arg::center(Cursor::Pos().x, 500), constants::paddle::SIZE.x, constants::paddle::SIZE.y }) {}

	void Draw() {
		paddle.rounded(3).draw();
	}

	void Update() {
		paddle.x = Cursor::Pos().x;
	}

	void Intersects(Ball* target) {
		using namespace constants::ball;
		// パドルとの衝突
		if ((0 < target->velocity.y) && paddle.intersects(target->ball)) {
			target->velocity = Vec2{
				(target->ball.x - paddle.center().x) * 10,
				-target->velocity.y,
			}.setLength(SPEED);
		}
	}
};

void Main()
{
	Paddle paddle;
	Ball ball;
	Bricks bricks;

	while (System::Update())
	{
#pragma region 更新
		paddle.Update();
		ball.Update();
#pragma endregion

#pragma region 衝突
		paddle.Intersects(&ball);
		bricks.Intersects(&ball);
#pragma endregion

#pragma region 描画
		ball.Draw();
		bricks.Draw();
		paddle.Draw();
#pragma endregion
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
