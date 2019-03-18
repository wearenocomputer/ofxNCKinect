#include "ncKinectv2GestureDetector.h"

ncKinectv2GestureDetector::~ncKinectv2GestureDetector() {
	SafeReleaseGDT(frameReader);
	SafeReleaseGDT(frameSource);
	SafeReleaseGDT(dataBase);
}

ncKinectv2GestureDetector::ncKinectv2GestureDetector(IKinectSensor* _sensor, int _id, string _database) {
	id = _id;
	sensor = _sensor;

	//create frame source
	CreateVisualGestureBuilderFrameSource(sensor, 0, &frameSource);

	//create reader with source
	frameSource->OpenReader(&frameReader);
	frameReader->put_IsPaused(TRUE);

	//load the database
	std::wstring stow(+_database.length(), L' '); // Make room for characters
	std::copy(_database.begin(), _database.end(), stow.begin());
	wstring sDatabaseFile = L"data/"+stow;

	if (CreateVisualGestureBuilderDatabaseInstanceFromFile(sDatabaseFile.c_str(), &dataBase) != S_OK) {
		wcerr << L"Can't read database file " << sDatabaseFile << endl;
	}

	//set the number of gestures
	dataBase->get_AvailableGesturesCount(&numberofgestures);
	gestures.resize(numberofgestures);
	//get the gestures
	dataBase->get_AvailableGestures(numberofgestures, &gestures[0]);

	GestureType gType;
	const UINT uTextLength = 260; // magic number, if value smaller than 260, can't get name
	wchar_t sName[uTextLength];

	//loop through the gestures
	for (int i = 0; i < numberofgestures; ++i) {

		if (frameSource->AddGesture(gestures[i]) != S_OK) {
			cout << "gesture not added to sourceframe" << endl;
		}

		const UINT uTextLength = 260;
		wchar_t sName[uTextLength];
		gestures[i]->get_Name(uTextLength, sName);
		wcout << "loaded gesture: "<<sName <<" for detector "<<id << endl;
	}
}

void ncKinectv2GestureDetector::update() {

	if (!getPaused()) {
		IVisualGestureBuilderFrame *gestureFrame;

		if (frameReader->CalculateAndAcquireLatestFrame(&gestureFrame) != S_OK) {
			SafeReleaseGDT(gestureFrame);
			return;
		}

		BOOLEAN bvalid;
		gestureFrame->get_IsTrackingIdValid(&bvalid);

		if ((bool)bvalid) {
			GestureType mType;
			const UINT uTextLength = 260;
			wchar_t sName[uTextLength];
			for (size_t g = 0; g < gestures.size(); g++) {
				// get gesture information
				gestures[g]->get_GestureType(&mType);
				gestures[g]->get_Name(uTextLength, sName);
				if (mType == GestureType_Discrete) {
					IDiscreteGestureResult* pGestureResult = nullptr;
					if (gestureFrame->get_DiscreteGestureResult(gestures[g], &pGestureResult) == S_OK) {

						BOOLEAN bDetected;
						pGestureResult->get_Detected(&bDetected);
						if (bDetected) {
							float fConfidence = 0.0f;
							pGestureResult->get_Confidence(&fConfidence);
							//wcout << "Detected Gesture " << sName << " - " << fConfidence << endl;

							wstring ws(sName);
							string str(ws.begin(), ws.end());

							NCGenericEventArg arg;
							arg.message = str;
							arg.value = fConfidence;
							arg.userid = id;
							ncKinectEventDispatcher::NCEVENTDISPATCHER.dispatchDiscreteGesture(arg);

						}
					}
					SafeReleaseGDT(pGestureResult);
				}
				else if (mType == GestureType_Continuous) {

					IContinuousGestureResult* pGestureResult = nullptr;

					if (gestureFrame->get_ContinuousGestureResult(gestures[g], &pGestureResult) == S_OK) {

						float progress;
						pGestureResult->get_Progress(&progress);

						//wcout << "Detected Gesture " << sName << " - " << progress << endl;

						wstring ws(sName);
						string str(ws.begin(), ws.end());
						NCGenericEventArg arg;
						arg.message = ofToString(str);
						arg.value = progress;
						arg.userid = id;
						ncKinectEventDispatcher::NCEVENTDISPATCHER.dispatchContinuousGesture(arg);

					}

					SafeReleaseGDT(pGestureResult);
				}
			}
		}
		SafeReleaseGDT(gestureFrame);
	}
}

UINT64 ncKinectv2GestureDetector::getTrackingId() {
	UINT64 idt;
	frameSource->get_TrackingId(&idt);
	return idt;
}

void ncKinectv2GestureDetector::setTrackingId(UINT64 _id) {

	UINT64 trid;
	frameSource->get_TrackingId(&trid);

	if (trid != _id) {
		frameSource->put_TrackingId(_id);
	}

}

bool ncKinectv2GestureDetector::getPaused() {
	BOOLEAN bpaused;
	frameReader->get_IsPaused(&bpaused);

	return (bool)bpaused;
}

void ncKinectv2GestureDetector::setPaused(bool _paused) {

	frameReader->put_IsPaused(_paused);
}
