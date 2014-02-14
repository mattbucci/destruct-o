#pragma once

//A quick shortcut to all the interop includes
#include "MemoryDataTypes.h"
#include "MdxDataTypes.h"

#include <fstream>

namespace MDX {

	template <class T>
	InteropChainType4 * DecodeInteropChain4(char * file, long fileSize, long * fileOffset, long * globalSequenceId) {
		T * km = (T*)(file+*fileOffset);
		*fileOffset += sizeof(T);

		InteropChainType4 * chain = NULL;

		*globalSequenceId = km->GlobalSeqId;

		switch (km->LineType) {
		case 0:
			chain = new InteropChain<StepInterop<quat>,quat>();
			break;
		
		case 3:
		case 2:
		case 1:
			chain = new InteropChain<QuatSlerpInterop<quat>,quat>();
			break;
		/*
		Todo implement squad
		case 2:
			chain = new InteropChain4<HermiteInterop>();
			break
		case 3:
			chain = new InteropChain<BezierInterop<vec4>,vec4>();
			break;;*/
		}

		/*
		ofstream * osf = NULL;
		if (km->LineType == 2)
			osf = new ofstream("go30.csv");

		int minFrame = 10000000;
		int maxFrame = 0;
		*/

		for (long i = 0; i < km->nunks; i++) {
			if (km->LineType <= 1) {
				T::KMT1 * kmt = (T::KMT1*)(file+*fileOffset);

				*fileOffset += sizeof(T::KMT1);
				
				quat a = quat(kmt->w,kmt->x,kmt->y,kmt->z);
				vec4 tx = vec4(kmt->x,kmt->y,kmt->z,kmt->w);
				quat * _b = (quat*)&tx;
				quat b = *_b;

				chain->AddValue(kmt->Frame,quat(kmt->w,kmt->x,kmt->y,kmt->z),quat(),quat());
			}
			else {
				
				T::KMT2 * kmt = (T::KMT2*)(file+*fileOffset);
				*fileOffset += sizeof(T::KMT2);
				/*
				minFrame = min(minFrame,(int)kmt->Frame);
				maxFrame = max(maxFrame,(int)kmt->Frame);
				if ((maxFrame >= 13333) && (maxFrame <= 13400))
					(*osf) << kmt->Frame << "," << kmt->x << "," << kmt->InTanx << "," << kmt->OutTanx <<"\n";

				*/
				chain->AddValue(kmt->Frame,
						quat(kmt->w, kmt->x,kmt->y,kmt->z),
						quat(kmt->InTanw, kmt->InTanx,kmt->InTany,kmt->InTanz),
						quat(kmt->OutTanw,kmt->OutTanx,kmt->OutTany,kmt->OutTanz));
			}
		}

		/*
		if (km->LineType == 2) {
			(*osf) << "NEXT ====\n";
			for (int i = minFrame; i < maxFrame; i+=1)
				if ((i >= 13333) && (i <= 13400))
					(*osf) << i << "," << chain->ValueAtSequence(i).x << "\n";
			osf->flush();
			osf->close();
			delete osf;
		}
		*/


		return chain;
	}

	template <class T>
	InteropChainType3 * DecodeInteropChain3(char * file, long fileSize, long * fileOffset, long * globalSequenceId) {
		T * km = (T*)(file+*fileOffset);
		*fileOffset += sizeof(T);

		InteropChainType3 * chain = NULL;

		*globalSequenceId = km->GlobalSeqId;

		switch (km->LineType) {
		case 0:
			chain = new InteropChain<StepInterop<vec3>,vec3>();
			break;
		case 1:
			chain = new InteropChain<LinearInterop<vec3>,vec3>();
			break;
		case 2:
			chain = new InteropChain<HermiteInterop<vec3>,vec3>();
			break;
		case 3:
			chain = new InteropChain<BezierInterop<vec3>,vec3>();
			break;
		default:
			return NULL;
		}

		for (long i = 0; i < km->nunks; i++) {
			if (km->LineType <= 1) {
				T::KMT1 * kmt = (T::KMT1*)(file+*fileOffset);

				*fileOffset += sizeof(T::KMT1);
				
				chain->AddValue(kmt->Frame,vec3(kmt->x,kmt->y,kmt->z),vec3(),vec3());
			}
			else {
				T::KMT2 * kmt = (T::KMT2*)(file+*fileOffset);
				*fileOffset += sizeof(T::KMT2);

				chain->AddValue(kmt->Frame,vec3(kmt->x,kmt->y,kmt->z),
						vec3(kmt->InTanx,kmt->InTany,kmt->InTanz),
						vec3(kmt->OutTanx,kmt->OutTany,kmt->OutTanz));
			}
		}
		return chain;
	}

	template <class T>
	InteropChainType1 * DecodeInteropChain(char * file, long fileSize, long * fileOffset, long * globalSequenceId) {
		T * km = (T*)(file+*fileOffset);
		*fileOffset += sizeof(T);

		InteropChainType1 * chain = NULL;

		*globalSequenceId = km->GlobalSeqId;

		switch (km->LineType) {
		case 0:
			chain = new InteropChain<StepInterop<float>,float>();
			break;
		case 1:
			chain = new InteropChain<LinearInterop<float>,float>();
			break;
		case 2:
			chain = new InteropChain<HermiteInterop<float>,float>();
			break;
		case 3:
			chain = new InteropChain<BezierInterop<float>,float>();
			break;
		}

		for (long i = 0; i < km->nunks; i++) {
			if (km->LineType <= 1) {
				T::KMT1 * kmt = (T::KMT1*)(file+*fileOffset);
				*fileOffset += sizeof(T::KMT1);

				chain->AddValue(kmt->Frame,kmt->State,0,0);
			}
			else {
				T::KMT2 * kmt = (T::KMT2*)(file+*fileOffset);
				*fileOffset += sizeof(T::KMT2);

				chain->AddValue(kmt->Frame,kmt->State,kmt->inTan,kmt->outTan);
			}
		}
		return chain;
	}
};