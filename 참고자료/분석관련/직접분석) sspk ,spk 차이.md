
spk 는 게임에서 사용하는 오브젝트들 이미지 모음이고,
sspk 는 shadow sprite 라는 명칭인듯함.

# 대부분의 객체는 아래 헤더파일에 멤버변수로 선언되어 있는듯 하고,
"H:\Source\GithubDesktop\dk_client\Client\MTopView.h"

# 예)

		//------------------------------------------------------		
		// Shadow SpritePack
		//------------------------------------------------------
		CShadowSpritePack		m_AddonSSPK;			// Sprite들		
		CShadowSpritePack		m_CreatureSSPK;
		CShadowSpritePack		m_OustersSSPK;
		CShadowSpritePack		m_ImageObjectSSPK;

		CShadowSpritePack		m_AdvancementSlayerManSSPK;			// Sprite들		
		CShadowSpritePack		m_AdvancementSlayerWomanSSPK;			// Sprite들		
		CShadowSpritePack		m_AdvancementVampireManSSPK;
		CShadowSpritePack		m_AdvancementVampireWomanSSPK;
		CShadowSpritePack		m_AdvancementOustersSSPK;

		
# 스프라이트 파일 대칭은 FileDef.inf 에서 확인이 가능하다. 
# InitGame() 함수 에서 읽고, g_pFileDef->load("Data/Info/FileDef.inf"); 으로 로드함 
"H:\Source\GithubDesktop\dk_client\build\vs2019\bin\Debug\Data\Info\FileDef.inf"

# 예)

FILE_SPRITE_TILE					 : Data\\Image\\Tile.spk
FILE_SPRITEINDEX_TILE				 : Data\\Image\\Tile.spki
FILE_SPRITE_IMAGEOBJECT				 : Data\\Image\\ImageObject.spk
FILE_SPRITEINDEX_IMAGEOBJECT		 : Data\\Image\\ImageObject.spki
FILE_SPRITE_INTERACTIONOBJECT		 : Data\\Image\\InteractionObject.spk
FILE_SPRITEINDEX_INTERACTIONOBJECT	 : Data\\Image\\InteractionObject.spki

FILE_SSPRITE_IMAGEOBJECT			 : Data\\Image\\ImageObject.sspk
FILE_SSPRITEINDEX_IMAGEOBJECT		 : Data\\Image\\ImageObject.sspki
FILE_SSPRITE_INTERACTIONOBJECT		 : Data\\Image\\InteractionObject.sspk
FILE_SSPRITEINDEX_INTERACTIONOBJECT	 : Data\\Image\\InteractionObject.sspki