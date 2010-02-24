#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

class OmLocalConfiguration {

public:
	static OmLocalConfiguration* Instance();
	static void Delete();

	static int get_num_cores();
	static int numAllowedWorkerThreads();

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmLocalConfiguration();
	~OmLocalConfiguration();
	OmLocalConfiguration(const OmLocalConfiguration&);
	OmLocalConfiguration& operator= (const OmLocalConfiguration&);

private:
	//singleton
	static OmLocalConfiguration* mspInstance;

};

#endif
