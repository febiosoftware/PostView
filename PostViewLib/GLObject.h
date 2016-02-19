#pragma once

class CGLContext;

//-----------------------------------------------------------------------------
// This class is the base class for anything that affects what's get rendered
// to the 3D view. 
//
class CGLObject
{
public:
	CGLObject();
	virtual ~CGLObject();

	// update contents
	virtual void Update(int ntime, float dt, bool breset) {}

	// get the name
	const char* GetName() const { return m_szname; }
	void SetName(const char* szname);

	// (de-)activate
	virtual void Activate(bool bact) { m_bactive = bact; }
	bool IsActive() { return m_bactive; }

protected:
	char	m_szname[64];
	bool	m_bactive;
};

//-----------------------------------------------------------------------------
// This is the base class for anything that can draw itself to the 3D view
//
class CGLVisual : public CGLObject
{
public:
	CGLVisual() { m_bclip = true; }

	// render the object to the 3D view
	virtual void Render(CGLContext& rc) = 0;

	// allow clipping
	bool AllowClipping() { return m_bclip; }
	void AllowClipping(bool b) { m_bclip = b; }

private:
	bool	m_bclip;	// allow the object to be clipped
};
