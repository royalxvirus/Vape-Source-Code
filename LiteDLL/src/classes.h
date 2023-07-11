#ifndef LITEDLL_CLASSES_H_
#define LITEDLL_CLASSES_H_

#include <jni.h>

// TODO: replace JNI types with wrappers.
// TODO: classes

typedef struct {
	jfieldID playerController;
	jfieldID pointedEntity;
	jfieldID objectMouseOver;
	jfieldID gameSettings;
	jfieldID currentScreen;
	jfieldID session;
	jfieldID timer;
	jfieldID mouseHelper;
	jfieldID rightClickDelayTimer;
	jfieldID thePlayer;
	jfieldID renderViewEntity;
	jfieldID renderManager;
	jfieldID leftClickCounter;
	jfieldID renderItem;
	jfieldID fontRendererObj;
	jfieldID theWorld;
	jfieldID inGameHasFocus;
	jfieldID entityRenderer;
	jfieldID displayWidth;
	jfieldID displayHeight;
	jfieldID mcProfiler;
	jmethodID getMinecraft;
	jmethodID displayGuiScreen;
	jmethodID getTextureManager;
	jmethodID setIngameFocus;
	jmethodID setIngameNotInFocus;
	jmethodID runGameLoop;
} Minecraft, * PMinecraft;

typedef struct {
	jmethodID stripControlCodes;
} StringUtils, * PStringUtils;

typedef struct {
	jfieldID boundingBox;
	jfieldID ridingEntity;
	jfieldID posX;
	jfieldID posY;
	jfieldID posZ;
	jfieldID rotationYaw;
	jfieldID rotationPitch;
	jfieldID entityId;
	jfieldID lastTickPosX;
	jfieldID lastTickPosY;
	jfieldID lastTickPosZ;
	jfieldID hurtResistantTime;
	jfieldID onGround;
	jfieldID prevPosX;
	jfieldID prevPosY;
	jfieldID prevPosZ;
	jfieldID ticksExisted;
	jmethodID getFlag;
	jmethodID setFlag;
	jmethodID getDisplayName;
	jfieldID height;
	jfieldID isDead;
	jfieldID motionX;
	jfieldID motionY;
	jfieldID motionZ;
	jmethodID getDistanceToEntity;
	jmethodID canBeCollidedWith;
	jmethodID getCollisionBorderSize;
	jmethodID isSneaking;
	jmethodID setPosition;
	jmethodID setSneaking;
	jmethodID getName;
	jmethodID setAngles;
	jmethodID getEyeHeight;
} Entity, * PEntity;

typedef struct {
	jfieldID isSwingInProgress;
	jfieldID absorptionAmount;
} EntityLivingBase, * PEntityLivingBase;

typedef struct {
	jmethodID rayTrace;
} Entity, * PEntity;

typedef struct {
	jmethodID getEntityAttribute;
} EntityLivingBase, * PEntityLivingBase;

typedef struct {
	jmethodID getPositionEyes;
	jmethodID getLook;
} Entity, * PEntity;

typedef struct {
	jmethodID getHealth;
	jmethodID getMaxHealth;
	jmethodID canEntityBeSeen;
	jmethodID getAbsorptionAmount;
} EntityLivingBase, * PEntityLivingBase;

typedef struct {
	jfieldID itemDamage;
	jfieldID stackSize;
	jmethodID getItem;
	jmethodID getEnchantmentTagList;
	jmethodID getDisplayName;
} ItemStack, * PItemStack;

typedef struct {
	jfieldID mainInventory;
	jfieldID armorInventory;
	jfieldID currentItem;
	jfieldID itemStack;
	jmethodID getCurrentItem;
} InventoryPlayer, * PInventoryPlayer;

typedef struct {
	jfieldID maxDamage;
	jmethodID getUnlocalizedName;
	jmethodID getItemFromBlock;
	jfieldID BLOCK_TO_ITEM;
} Item, * PItem;

typedef struct {
	jfieldID inventory;
	jfieldID chasingPosX; // field_71094_bP
	jfieldID chasingPosY; // field_71095_bQ
	jfieldID chasingPosZ; // field_71085_bR
	jfieldID prevChasingPosX; // field_71091_bM
	jfieldID prevChasingPosY; // field_71096_bN
	jfieldID prevChasingPosZ; // field_71097_bO
	jfieldID openContainer;
	jmethodID getWorldScoreboard;
	jmethodID getTeam;
	jmethodID closeScreen;
	jmethodID getCurrentEquippedItem;
} EntityPlayer, * PEntityPlayer;

typedef struct {
	jfieldID movementInput;
} EntityPlayerSP, * PEntityPlayerSP;

typedef struct {
	jfieldID pressTime;
	jfieldID pressed;
	jfieldID keyCode;
	jmethodID setKeyBindState;
	jmethodID onTick;
} KeyBinding, * PKeyBinding;

typedef struct {
	jmethodID getEntityByID;
	jmethodID getBlockState;
	jfieldID theProfiler;
	jfieldID loadedEntityList;
	jmethodID getCollidingBoundingBoxes;
	jmethodID getEntitiesWithinAABBExcludingEntity;
	jfieldID loadedTileEntityList;
} World, * PWorld;

typedef struct {
	jfieldID isHittingBlock;
	jmethodID getBlockReachDistance;
	jmethodID extendedReach;
	jmethodID windowClick;
} PlayerControllerMP, * PPlayerControllerMP;

typedef struct {
	jfieldID hitVec;
	jfieldID typeOfHit;
} MovingObjectPosition, * PMovingObjectPosition;

typedef struct {
	jfieldID xCoord;
	jfieldID yCoord;
	jfieldID zCoord;
} Vec3, * PVec3;

typedef struct {
	jmethodID enableLightmap;
	jmethodID disableLightmap;
	jfieldID farPlaneDistance;
	jfieldID pointedEntity;
	jmethodID setupCameraTransform;
} EntityRenderer, * PEntityRenderer;

typedef struct {
	jfieldID minX;
	jfieldID minY;
	jfieldID minZ;
	jfieldID maxX;
	jfieldID maxY;
	jfieldID maxZ;
	jmethodID calculateIntercept;
	jmethodID offset;
} AxisAlignedBB, * PAxisAlignedBB;

typedef struct {
	jfieldID MISS;
	jfieldID BLOCK;
} MovingObjectPosition$MovingObjectType, * PMovingObjectPosition$MovingObjectType;

typedef struct {
	jmethodID isSameTeam;
} Team, * PTeam;

typedef struct {
	jfieldID moveForward;
	jfieldID moveStrafe;
	jfieldID sneak;
} MovementInput, * PMovementInput;

typedef struct {
	jfieldID limitFramerate;
	jfieldID gammaSetting;
	jfieldID mouseSensitivity;
	jfieldID keyBindAttack;
	jfieldID keyBindDrop;
	jfieldID keyBindInventory;
	jfieldID keyBindForward;
	jfieldID keyBindSprint;
	jfieldID keyBindSneak;
	jfieldID viewBobbing;
	jfieldID keyBindUseItem;
	jfieldID guiScale;
} GameSettings, * PGameSettings;

typedef struct {
	jmethodID getEffects;
} ItemPotion, * PItemPotion;

typedef struct {
	jfieldID playerViewX;
	jfieldID playerViewY;
} RenderManager, * PRenderManager;

typedef struct {
	jmethodID getIdFromBlock;
} Block, * PBlock;

typedef struct {
	jfieldID lowerChestInventory;
	jmethodID drawGuiContainerForegroundLayer;
} GuiChest, * PGuiChest;

typedef struct {
	jmethodID getUnformattedText;
	jmethodID getFormattedText;
} IChatComponent, * PIChatComponent;

typedef struct {
	jmethodID drawString; //func_78276_b
	jmethodID getStringWidth;
} FontRenderer, * PFontRenderer;

typedef struct {
	jfieldID allowUserInput;
	jfieldID width;
	jfieldID height;
} GuiScreen, * PGuiScreen;

typedef struct {
	jfieldID theSlot;
	jmethodID getSlotAtPosition;
	jfieldID guiLeft;
	jfieldID guiTop;
	jfieldID xSize;
	jfieldID ySize;
	jfieldID inventorySlots;
} GuiContainer, * PGuiContainer;

typedef struct {
	jfieldID slotNumber;
	jfieldID xDisplayPosition;
	jfieldID yDisplayPosition;
	jmethodID getStack;
} Slot, * PSlot;

typedef struct {
	jfieldID username;
} Session, * PSession;

typedef struct {
	jmethodID startSection;
	jmethodID endStartSection;
	jfieldID profilingEnabled;
	jfieldID profilingSection;
	jfieldID sectionList;
	jfieldID timestampList;
} Profiler, * PProfiler;

typedef struct {
	jfieldID renderPartialTicks;
} Timer, * PTimer;

typedef struct {
	jfieldID teamMemberships;
} Scoreboard, * PScoreboard;

typedef struct {
	jfieldID zLevel;
	jmethodID renderItemIntoGUI;
	jmethodID renderItemOverlays;
} RenderItem, * PRenderItem;

typedef struct {
	jmethodID enableStandardItemLighting;
	jmethodID disableStandardItemLighting;
} RenderHelper, * PRenderHelper;

typedef struct {
	jmethodID tagCount;
	jmethodID getCompoundTagAt;
} NBTTagList, * PNBTTagList;

typedef struct {
	jmethodID getShort;
} NBTTagCompound, * PNBTTagCompound;

typedef struct {
	jfieldID effectId;
	jfieldID protection;
	jfieldID unbreaking;
	jfieldID sharpness;
	jfieldID fireAspect;
	jfieldID efficiency;
	jfieldID featherFalling;
	jfieldID power;
	jfieldID flame;
	jfieldID punch;
	jfieldID fortune;
	jfieldID infinity;
	jfieldID thorns;
	jfieldID knockback;
	jfieldID enchantmentsList;
	jmethodID getTranslatedName;
} Enchantment, * PEnchantment;

typedef struct {
	jmethodID getSizeInventory;
	jmethodID getStackInSlot;
} IInventory, * PIInventory;

typedef struct {
	jfieldID windowId;
	jfieldID inventorySlots;
} Container, * PContainer;

typedef struct {
	jfieldID pos;
} TileEntity, * PTileEntity;

typedef struct {
	jfieldID lidAngle;
	jfieldID cachedChestType;
} TileEntityChest, * PTileEntityChest;

typedef struct {
	jmethodID getBlock;
} IBlockState, * PIBlockState;

typedef struct {
	jmethodID bindTexture;
	jmethodID enableLighting;
	jmethodID disableLighting;
	jmethodID depthMask;
	jmethodID enableBlend;
	jmethodID disableBlend;
	jmethodID color; // func_179131_c
	jmethodID disableAlpha;
	jmethodID enableAlpha;
	jmethodID disableDepth;
	jmethodID tryBlendFuncSeparate;
	jmethodID enableTexture2D;
	jmethodID disableTexture2D;
	jmethodID enableDepth;
} GlStateManager, * PGlStateManager;

typedef struct {
	jmethodID getName;
	jmethodID hasCustomName;
	jmethodID getDisplayName;
} IWorldNameable, * PIWorldNameable;

typedef struct {
	jfieldID x;
	jfieldID y;
	jfieldID z;
};

#endif