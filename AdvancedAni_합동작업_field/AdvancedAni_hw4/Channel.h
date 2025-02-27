

#pragma once
namespace kicker {
	class Channel {
		typedef int Index;
	public:
		enum ChannelEnum {
			X_ROTATION, Y_ROTATION, Z_ROTATION,
			X_POSITION, Y_POSITION, Z_POSITION,
		};
		Index          joint;
		ChannelEnum    type;
		Index          index;
	};
}

namespace keeper {
	class Channel {
		typedef int Index;
	public:
		enum ChannelEnum {
			X_ROTATION, Y_ROTATION, Z_ROTATION,
			X_POSITION, Y_POSITION, Z_POSITION,
		};
		Index          joint;
		ChannelEnum    type;
		Index          index;
	};
}