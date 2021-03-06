#include <LightOrbColors.h>


// Red Color
DirectX::PackedVector::XMCOLOR C_RED = DirectX::PackedVector::XMCOLOR{0xFFFF0900};
DirectX::PackedVector::XMCOLOR C_RED_EDGE = DirectX::PackedVector::XMCOLOR{0xFFFF4A00};

// Red Ring Color
DirectX::PackedVector::XMCOLOR C_RED_RING = C_RED;

// Green Color
DirectX::PackedVector::XMCOLOR C_GREEN = DirectX::PackedVector::XMCOLOR{0xFF00FF00};
DirectX::PackedVector::XMCOLOR C_GREEN_EDGE = DirectX::PackedVector::XMCOLOR{0xFF00EE00};

// Green Ring Color
DirectX::PackedVector::XMCOLOR C_GREEN_RING = C_GREEN;

// Blue Color
DirectX::PackedVector::XMCOLOR C_BLUE = DirectX::PackedVector::XMCOLOR{0xFF003AFF};
DirectX::PackedVector::XMCOLOR C_BLUE_EDGE = DirectX::PackedVector::XMCOLOR{0xFF008ADD};

// Blue Ring Color
DirectX::PackedVector::XMCOLOR C_BLUE_RING = C_BLUE;

// Yellow Color
DirectX::PackedVector::XMCOLOR C_YELLOW = DirectX::PackedVector::XMCOLOR{0xFFFFFF00};

// Cyan Color
DirectX::PackedVector::XMCOLOR C_CYAN = DirectX::PackedVector::XMCOLOR{0xFF00FFFF};

// Magenta Color
DirectX::PackedVector::XMCOLOR C_MAGENTA = DirectX::PackedVector::XMCOLOR{0xFFFF00FF};

// Orange Color
DirectX::PackedVector::XMCOLOR C_ORANGE = DirectX::PackedVector::XMCOLOR{0xFFFF7F00};

// White Color
DirectX::PackedVector::XMCOLOR C_WHITE = DirectX::PackedVector::XMCOLOR{0xFFFFFFFF};

// Black Color
DirectX::PackedVector::XMCOLOR C_BLACK = DirectX::PackedVector::XMCOLOR{0xFF000000};

// BGRA colors if separate
const DirectX::PackedVector::XMCOLOR E_LIGHT_ORBS::ORB_COLORS[E_LIGHT_ORBS::COUNT] = {C_RED, C_GREEN, C_BLUE, C_WHITE};
const DirectX::PackedVector::XMCOLOR E_LIGHT_ORBS::ORB_EDGE_COLORS[E_LIGHT_ORBS::COUNT] = {C_RED_EDGE, C_GREEN_EDGE, C_BLUE_EDGE, C_WHITE};
const DirectX::PackedVector::XMCOLOR E_LIGHT_ORBS::RING_COLORS[3] = {C_RED_RING, C_GREEN_RING, C_BLUE_RING};

const KeyCode E_LIGHT_ORBS::ColorInputKeyCodes[E_LIGHT_ORBS::COUNT] = {KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::Any};

