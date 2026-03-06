#include <hidl/HidlSupport.h>
#include <hidl/Static.h>
#include <dlfcn.h>
#include <android-base/logging.h>

namespace android {
namespace hardware {

// legacy constructors
namespace details {
    DoNotDestruct<BnConstructorMap> gBnConstructorMap{};
    DoNotDestruct<BsConstructorMap> gBsConstructorMap{};
}

typedef sp<IBinder> (*getOrCreateCachedBinder_t)(::android::hidl::base::V1_0::IBase*);

sp<IBinder> getOrCreateCachedBinder(::android::hidl::base::V1_0::IBase* ifacePtr) {
    if (ifacePtr != nullptr && !ifacePtr->isRemote()) {
        std::string descriptor;
        auto ret = ifacePtr->interfaceDescriptor([&](const auto& desc) {
            descriptor = desc.c_str();
        });

        if (ret.isOk() && !descriptor.empty()) {
            auto func = details::getBnConstructorMap().get(descriptor, nullptr);

            if (!func) {
                LOG(ERROR) << "libhidl_shim_full: " << descriptor << " uses legacy constructor map";

                func = details::gBnConstructorMap->get(descriptor, nullptr);

                if (func) {
                    details::getBnConstructorMap().set(std::move(descriptor), std::move(func));
                }
            }
        }
    }

    // Due to us being this function we need to look it up. This is a bit of a HACK.
    static getOrCreateCachedBinder_t original_func = reinterpret_cast<getOrCreateCachedBinder_t>(
        dlsym(RTLD_NEXT, "_ZN7android8hardware23getOrCreateCachedBinderEPNS_4hidl4base4V1_05IBaseE")
    );

    return original_func(ifacePtr);
}

} // namespace hardware
} // namespace android
