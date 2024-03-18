#ifndef __SDCARD_STATUS_H
#define __SDCARD_STATUS_H

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

		void getTotal(char* total)
		{
			if (this->getTotalGBytes() > 0) {
				sprintf(total, "%ld%s", this->getTotalGBytes(), "G");
				return;
			}

			sprintf(total, "%ld%s", this->getTotalMBytes(), "M");
		}

		void getFree(char* free)
		{
			if (this->getFreeGBytes() > 0) {
				sprintf(free, "%ld%s", this->getFreeGBytes(), "G");
				return;
			}

			sprintf(free, "%ld%s", this->getFreeMBytes(), "M");
		}

		void getUsed(char* used)
		{
			if (this->getUsedGBytes() > 0) {
				sprintf(used, "%ld%s", this->getUsedGBytes(), "G");
				return;
			}

			sprintf(used, "%ld%s", this->getUsedMBytes(), "M");
		}
};

#endif
