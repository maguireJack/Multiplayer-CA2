#include "ShakeEffect.hpp"

#include <SFML/Graphics/Shader.hpp>

ShakeEffect::ShakeEffect()
{
	m_shaders.Load(ShaderTypes::kShakePass, "Media/Shaders/Shake.vert", "Media/Shaders/Shake.frag");
}

void ShakeEffect::Apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	m_counter += 0.016;
	Apply(input, output, sf::seconds(m_counter), 1);
}

void ShakeEffect::Apply(const sf::RenderTexture& input, sf::RenderTarget& output, sf::Time time, float intensity)
{
	sf::Shader& shake = m_shaders.Get(ShaderTypes::kShakePass);
	
	shake.setUniform("time", time.asMilliseconds() / 1000.f);
	shake.setUniform("intensity", intensity);

	shake.setUniform("source", input.getTexture());
	
	shake.setUniformArray("offsets", vectorArray, 9);

	float blur_kernel[9] = {
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};

	shake.setUniformArray("blur_kernel", blur_kernel, 9);

	ApplyShader(shake, output);
}


