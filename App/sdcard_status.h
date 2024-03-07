#ifndef __SDCARD_STATUS_H
#define __SDCARD_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif


struct SDCardStatus
{
	private:
		uint64_t totalMBytes = 0;
		uint64_t freeMBytes = 0;
		uint64_t usedMBytes = 0;

	public:
		SDCardStatus(uint64_t totalMBytes, uint64_t freeMBytes, uint64_t usedMBytes)
		{
			this->totalMBytes = totalMBytes;
			this->freeMBytes = freeMBytes;
			this->usedMBytes = usedMBytes;
		}

		uint32_t getTotalMBytes()
		{
			return totalMBytes;
		}

		uint32_t getFreeMBytes()
		{
			return freeMBytes;
		}

		uint32_t getUsedMBytes()
		{
			return usedMBytes;
		}

		uint32_t getTotalGBytes()
		{
			return totalMBytes/1024;
		}

		uint32_t getFreeGBytes()
		{
			return freeMBytes/1024;
		}

		uint32_t getUsedGBytes()
		{
			return usedMBytes/1024;
		}
};



#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_STATUS_H */
